#include "../include/access_private.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

#define ASSERT(CONDITION)                                                      \
  do                                                                           \
    if (!(CONDITION)) {                                                        \
      printf("Assertion failure %s:%d ASSERT(%s)\n", __FILE__, __LINE__,       \
             #CONDITION);                                                      \
      abort();                                                                 \
    }                                                                          \
  while (0)

class A {
  int m_i = 3;
  int m_f(int p) { return 14 * p; }
  constexpr int m_cxf(int p) const { return p * m_i; }
  static int s_i;
  static const int s_ci = 403;
  static constexpr const int s_cxi = 42;
  static int s_f(int r) { return r + 1; }
  constexpr static int s_cxf(int r) { return r * 2; }

public:
  const int &get_m_i() const { return m_i; }
  static const int &get_s_i() { return s_i; }
};
int A::s_i = 404;
// Because we are using a pointer in the implementation, we need to explicitly
// define the const static variable as well, otherwise we'll have linker error.
const int A::s_ci;

template struct access_private::access<&A::m_i>;

void test_access_private_in_lvalue_expr() {
  A a;
  auto &i = access_private::accessor<"m_i">(a);
  ASSERT(i == 3);
  ++i;
  ASSERT(a.get_m_i() == 4);
}
void test_access_private_in_prvalue_expr() {
  auto i = access_private::accessor<"m_i">(A{});
  ASSERT(i == 3);
}
void test_access_private_in_xvalue_expr() {
  A a;
  auto i = access_private::accessor<"m_i">(std::move(a));
  ASSERT(i == 3);
}

namespace NS {
class B {
  int m_i = 3;

public:
  class C {
    int m_i = 4;
  };
};
} // NS

namespace access_private {
  template struct access<&NS::B::m_i>;
  template struct access<&NS::B::C::m_i>;
}

void test_access_private_in_class_in_namespace() {
  NS::B b;
  auto &i = access_private::accessor<"m_i">(b);
  ASSERT(i == 3);
}


void test_access_private_in_nested_class() {
  NS::B::C c;
  auto &i = access_private::accessor<"m_i">(c);
  ASSERT(i == 4);
}

class C {
  const int m_i = 3;
};

namespace access_private {
  template struct access<&C::m_i>;
}

void test_access_private_const_member() {
  C c;
  auto &i = access_private::accessor<"m_i">(c);
  // should not deduce to int&
  static_assert(std::is_same<const int &, decltype(i)>::value, "");
  ASSERT(i == 3);
}

class CA {
  int m_i = 3;

public:
  CA() {}
};

namespace access_private {
  template struct access<&CA::m_i>;
}

void test_access_private_const_object() {
  const CA ca;
  auto &i = access_private::accessor<"m_i">(ca);
  // should not deduce to int&
  static_assert(std::is_same<const int &, decltype(i)>::value, "");
  ASSERT(i == 3);
}

template <typename T>
class TemplateA {
  T m_i = 3;
};


namespace access_private {
  template struct access<&TemplateA<int>::m_i>;
}

void test_access_private_template_field() {
  TemplateA<int> a;
  auto &i = access_private::accessor<"m_i">(a);
  ASSERT(i == 3);
}

void test_access_private_constexpr() {
  constexpr A a;
  static_assert(access_private::accessor<"m_i">(a) == 3, "");
}

namespace access_private {
  template struct access<&A::m_f>;
}

void test_call_private_in_lvalue_expr() {
  A a;
  int p = 3;
  auto res = access_private::accessor<"m_f">(a, std::move(p));
  ASSERT(res == 42);
}
void test_call_private_in_prvalue_expr() {
  auto res = access_private::accessor<"m_f">(A{}, 3);
  ASSERT(res == 42);
}
void test_call_private_in_xvalue_expr() {
  A a;
  auto res = access_private::accessor<"m_f">(std::move(a), 3);
  ASSERT(res == 42);
}

namespace access_private {
  template struct access<&A::m_cxf>;
}

void test_call_private_constexpr() {
  constexpr A a;
  static_assert(access_private::accessor<"m_cxf">(a, 5) == 15, "");
}

namespace access_private {
  template struct access<&A::s_i, A>;
}

void test_access_private_static() {
  auto &i = access_private::accessor<"s_i">.static_ref<A>();
  ASSERT(i == 404);
  ++i;
  ASSERT(A::get_s_i() == 405);
}


namespace access_private {
  template struct access<&A::s_ci, A>;
}

void test_access_private_static_const() {
  auto &i = access_private::accessor<"s_ci">.static_ref<A>();
  static_assert(std::is_same<const int &, decltype(i)>::value, "");
  ASSERT(i == 403);
}

namespace access_private {
  template struct access<&A::s_cxi, A>;
}

void test_access_private_static_constexpr() {
  static_assert(access_private::accessor<"s_cxi">.static_ref<A>() == 42, "");
}

namespace access_private {
  template struct access<&A::s_f, A>;
}

void test_call_private_static() {
  auto l = access_private::accessor<"s_f">.call<A>(4);
  ASSERT(l == 5);
}

namespace access_private {
  template struct access<&A::s_cxf, A>;
}

void test_call_private_static_constexpr() {
  static_assert(access_private::accessor<"s_cxf">.call<A>(5) == 10, "");
}

class A3 {
  int m_i = 3;
  int m_f(int x) { return x + m_i; }
  int m_f(int x, int y) { return x + y * m_i; }
  template <typename T>
  constexpr auto m_cxf(T x) const -> decltype(x + m_i) {
    return x + m_i;
  }
  template <typename T, typename U>
  static auto s_f(T t, U u) -> decltype(t + u) {
    return t + u;
  }
  constexpr static const char nums[] = "0123456789";
  constexpr static char s_cxf(int a) {
    return nums[a];
  }
  constexpr static const char* s_cxf(float f) {
    return nums + static_cast<int>(f * sizeof(nums) / sizeof(nums[0]));
  }
};

#include "../include/overload.hpp"

namespace access_private {
  template struct access<overload<int>(&A3::m_f)>;
  template struct access<overload<int, int>(&A3::m_f)>;
}

void test_call_private_overloaded() {
  auto res = access_private::accessor<"m_f">(A3(), 1);
  ASSERT(res == 4);
  res = access_private::accessor<"m_f">(A3(), 1, 2);
  ASSERT(res == 7);
}

using const_a3 = const A3;

namespace access_private {
  template struct access<&A3::m_cxf<int>>;
  template struct access<&A3::m_cxf<const char*>>;
}

void test_call_private_overloaded_constexpr() {
  constexpr A3 a3;
  static_assert(access_private::accessor<"m_cxf">(a3, 10) == 13, "");
  constexpr const char data[] = "hello world";
  static_assert(access_private::accessor<"m_cxf">(a3, &*data) == data+3, "");
}

namespace access_private {
  template struct access<&A3::s_f<char, int>, A3>;
  template struct access<&A3::s_f<const char*, std::string>, A3>;
}

void test_call_private_overloaded_static() {
  auto c = access_private::accessor<"s_f">.call<A3>('A', 25);
  ASSERT(c == 'Z');
  auto s = access_private::accessor<"s_f">.call<A3>(&*"Hello", std::string{"World"});
  ASSERT(s == "HelloWorld");
}

namespace access_private {
  template struct access<overload<int>(&A3::s_cxf), A3>;
  template struct access<overload<float>(&A3::s_cxf), A3>;
}

void test_call_private_overloaded_static_constexpr() {
  static_assert(access_private::accessor<"s_cxf">.call<A3>(3) == '3', "");
  static_assert(*access_private::accessor<"s_cxf">.call<A3>(0.5f) == '5', "");
}


int main() {
  test_access_private_in_lvalue_expr();
  test_access_private_in_prvalue_expr();
  test_access_private_in_xvalue_expr();
  test_access_private_in_class_in_namespace();
  test_access_private_in_nested_class();
  test_access_private_const_member();
  test_access_private_const_object();
  test_access_private_template_field();
  test_access_private_constexpr();
  test_call_private_in_prvalue_expr();
  test_call_private_in_xvalue_expr();
  test_call_private_in_lvalue_expr();
  test_call_private_constexpr();
  test_access_private_static();
  test_access_private_static_const();
  test_access_private_static_constexpr();
  test_call_private_static();
  test_call_private_static_constexpr();
  test_call_private_overloaded();
  test_call_private_overloaded_constexpr();
  test_call_private_overloaded_static();
  test_call_private_overloaded_static_constexpr();
  printf("OK\n");
  return 0;
}

