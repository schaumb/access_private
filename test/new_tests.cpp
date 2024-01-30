#include "../include/access_private.hpp"
#include "../include/overload.hpp"

#include <cstdio>

class A {
  auto x(int, float) { return 4; }

  void y(double) { }
  auto y(int, float) const { return 45; }

  int z(int) { return 2;}
  double z(int) const { return 0.1;}
  void z(int) const volatile {}

  void def(int, float = 0.0f) {}
  void def2(int = 0, float = 0.0f) {}

  int a;

  struct X {};
  X d;
};

class B {
  B() = default;
  B(int, double) {}

  float z(int) const { return 0.21; }

  static void y(double) { cica(); }
  static auto y(int, float) { return 45; }
  static void cica() {}

  static void def(int, float = 0.0f) {}
  static void def2(int = 0, float = 0.0f) {}

  static constexpr const auto LAMBDA = [] { };
  static const int maca{};
};


class C {
  C() { printf("C()\n"); }
  C(const C&) { printf("C(const C&)\n"); }
  ~C() { printf("~C()\n"); }
};

namespace access_private {
  // access member variable
  template struct access<&A::a>;

  // access member function
  template struct access<&A::x>;

  // access parameter overloaded function
  // template struct access<&A::y>; will not work
  template struct access<overload<double>(&A::y)>;
  template struct access<overload<int, float>(&A::y)>;

  // access overloaded with specifier:
  template struct access<overload<int>(&A::z)>;
  template struct access<const_overload<int>(&A::z)>;
  // same as:
  // template struct access<man_overload<acc::const_, int>(&A::z)>;
  template struct access<man_overload<acc::volatile_, int>(&A::z)>;
  // or acc::const_volatile

  // other class with same name
  template struct access<&B::z>;

  // private type
  template struct type_access<A, A::X>;
  template struct access<&A::d>;


#if defined(__clang__) || defined(_MSC_VER)
#define HAS_STATIC_MEMBER_FUNCTION 1
#elif defined(__GNUC__)
#if __GNUC__ > 13 || (__GNUC__ == 13 && __GNUC_MINOR__ >= 2)
#define HAS_STATIC_MEMBER_FUNCTION 1
#else
#define HAS_STATIC_MEMBER_FUNCTION 0
#endif
#endif

#if HAS_STATIC_MEMBER_FUNCTION
  // static member function
  template struct access<&B::cica, B>;

  // overloaded static member function
  template struct access<overload<double>(&B::y), B>;
  template struct access<overload<int, float>(&B::y), B>;
#endif

  // static member variables
  template struct access<&B::LAMBDA, B>;
  // template struct access<&B::maca, B>;

#if not defined(_MSC_VER)
  // constructor
  template struct access<constructor(B), B>;

  template struct access<constructor(B, int, double), B>;


  template struct access<constructor(C, const C&), C>;

  // construction and destruction
  template struct access<constructing_at(C), C>;
  template struct access<constructing_at(C, const C&), C>;
  template struct access<destructing_at(C), C>;

  // unique parameters and default parameters
  call_member_function_with(A, def, unsigned int);
  call_static_function_with(B, def, long int);

  call_member_function_with(A, def2);
  call_member_function_with(A, def2, int);
  call_member_function_with(A, def2, int, float);
  call_static_function_with(B, def2);
  call_static_function_with(B, def2, int);
  call_static_function_with(B, def2, int, float);
#endif
}
int main() {
  using namespace access_private;
  constexpr std::array ptr = {
      // returns with the same mem-ptrs
      accessor<"a">.ptr<A*>(),
      accessor<"a">.ptr<const A*>(),
      accessor<"a">.ptr<volatile A*>(),
      accessor<"a">.ptr<A&>(),
  };

  constexpr std::array ptrs2 = {
      // returns with the same mem funcptr
      accessor<"x">.ptr<A*, int, float>(),
      accessor<"x">.ptr<A&, int, float>(),
  };

  A a;
  auto& aa = accessor<"a">(&a);

  auto c1 = accessor<"x">(a, 1, 4.3f);

  // accessor<"x">(a, 1, 4.3); not work because 2nd parameter must be match with function argument type

  constexpr auto accing = accessor<"x">.ptr<A*, int, float>();
  auto vv = std::invoke(accing, a, 1, 4.3); // call with double


  accessor<"y">(a, 0.0);

  auto c2 = accessor<"y">(std::as_const(a), 1, 4.3f);

  auto c3 = accessor<"z">(std::move(a), 1);
  auto c4 = accessor<"z">(std::as_const(a), 1);


  float (B::*bx)(int) const = accessor<"z">.ptr<const B&, int>();

  using XT = type_accessor<A, "X">;

  XT copy_d = accessor<"d">(a);

  constexpr auto& p = accessor<"LAMBDA">.static_ref<B>();
  constexpr auto* pp = accessor<"LAMBDA">.static_ptr<B>();
  // auto p2 = accessor<"maca">.static_ref<B>(); // only copy is possible
  static_assert(&p == pp);
  p();

#if HAS_STATIC_MEMBER_FUNCTION
  accessor<"cica">.call<B>();

  constexpr void (*sp)(double) = accessor<"y">.static_ptr<B, double>();

  accessor<"y">.call<B>(0.1);
#endif

#if not defined(_MSC_VER)
  B X {accessor<"construct">.call<B>()};
  B X2 = accessor<"construct">.call<B>(1, 2.2);

  union U {
    char arr;
    C c;

    U() : arr{} {};
    ~U() {}
  };
  U u;
  U oth;

  C& c = accessor<"C">.call<C>(&u.c);
  C& othC = accessor<"C">.call<C>(&oth.c, std::as_const(c));

  auto constr = accessor<"construct">.static_ptr<C, const C&>();
  // C &&anoth {constr(std::as_const(c))}; // this will not work, because ~C is private

  accessor<"~C">.call<C>(othC);
  accessor<"~C">.call<C>(c);

  accessor<"def">(a, 4u); // calls a.def(4) with second default argument
  accessor<"def2">(a);
  accessor<"def2">(a, 1);
  accessor<"def2">(a, 1, 2.2f);

  accessor<"def">.call<B>(4l);
  accessor<"def2">.call<B>();
  accessor<"def2">.call<B>(1);
  accessor<"def2">.call<B>(1, 2.2f);
#endif

}