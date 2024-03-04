#include "../include/access_private.hpp"
#include "../include/overload.hpp"

#include <cstdio>
#include <cstdarg>

class A {
  auto x(int, float) { return 4; }

  void y(double) { }
  [[nodiscard]] auto y(int, float) const { return 45; }

  int z(int) { return 2;}
  double z(int) const { return 0.1;}
  void z(int) const volatile {}
  void z(int) volatile noexcept {}

  void def(int, float = 0.0f) {}
  void def2(int = 0, float = 0.0f) {}

  void print(const char* f, ...) {
    va_list list;
    va_start(list, f);
    std::vprintf(f, list);
    va_end(list);
  }

  static void print_stat(const char* f, ...) {
    va_list list;
    va_start(list, f);
    std::vprintf(f, list);
    va_end(list);
  }

  int a;

  struct X {};
  X d;
};

class B : A {
  B() = default;
  B(int, double) {}

  float z(int) const { return 0.21; }

  static void y(double) { cica(); }
  static auto y(int, float) { return 45; }
  static void cica() {}

  static void def(int, float = 0.0f) {}
  static void def2(int = 0, float = 0.0f) {}

  static constexpr auto LAMBDA = [] { };
  static const int maca{};
};


class C {
  C() { printf("C()\n"); }
  C(const C&) { printf("C(const C&)\n"); }
  ~C() { printf("~C()\n"); }
};

auto lambda_getter(int a) {
  return [a, b=10] () -> int { return a; };
}

auto lambda = lambda_getter(5);

class ops_x {
  using ops = ops_x;
  constexpr void operator+() const noexcept{}
  constexpr void operator-() const noexcept{}
  constexpr void operator&() const noexcept{}
  constexpr void operator*() const noexcept{}
  constexpr void operator~() const noexcept {}
  constexpr void operator!() const noexcept {}
  constexpr void operator++() const noexcept {}
  constexpr void operator++(int) const noexcept {}
  constexpr void operator--() const noexcept {}
  constexpr void operator--(int) const noexcept {}
  constexpr void operator+(const ops&) const noexcept {}
  constexpr void operator-(const ops&) const noexcept {}
  constexpr void operator*(const ops&) const noexcept {}
  constexpr void operator/(const ops&) const noexcept {}
  constexpr void operator%(const ops&) const noexcept {}
  constexpr void operator^(const ops&) const noexcept {}
  constexpr void operator&(const ops&) const noexcept {}
  constexpr void operator|(const ops&) const noexcept {}
  constexpr void operator=(const ops&) const noexcept {}
  constexpr void operator<(const ops&) const noexcept {}
  constexpr void operator>(const ops&) const noexcept {}
  constexpr void operator+=(const ops&) const noexcept {}
  constexpr void operator-=(const ops&) const noexcept {}
  constexpr void operator*=(const ops&) const noexcept {}
  constexpr void operator/=(const ops&) const noexcept {}
  constexpr void operator%=(const ops&) const noexcept {}
  constexpr void operator^=(const ops&) const noexcept {}
  constexpr void operator&=(const ops&) const noexcept {}
  constexpr void operator|=(const ops&) const noexcept {}
  constexpr void operator<<(const ops&) const noexcept {}
  constexpr void operator>>(const ops&) const noexcept {}
  constexpr void operator>>=(const ops&) const noexcept {}
  constexpr void operator<<=(const ops&) const noexcept {}
  constexpr void operator==(const ops&) const noexcept {}
  constexpr void operator!=(const ops&) const noexcept {}
  constexpr void operator<=(const ops&) const noexcept {}
  constexpr void operator>=(const ops&) const noexcept {}
  constexpr void operator<=>(const ops&) const noexcept {}
  constexpr void operator&&(const ops&) const noexcept {}
  constexpr void operator||(const ops&) const noexcept {}
  constexpr void operator,(const ops&) const noexcept {}
  constexpr void operator->*(const ops&) const noexcept {}
  constexpr void operator()(const ops&) const noexcept {}
  constexpr void operator[](const ops&) const noexcept {}
  constexpr const ops* operator->() const noexcept { return nullptr; }
  explicit operator A() const noexcept { return A{}; }
  explicit operator A&&() const noexcept { static A a; return std::move(a); }
};

namespace access_private {
  using ops = ops_x;

  template struct access<overload<>(&ops::operator+)>;
  template struct access<overload<>(&ops::operator-)>;
  template struct access<overload<>(&ops::operator&)>;
  template struct access<overload<>(&ops::operator*)>;
  template struct access<&ops::operator~>;
  template struct access<&ops::operator!>;
  template struct access<overload<>(&ops::operator++)>;
  template struct access<overload<int>(&ops::operator++)>;
  template struct access<overload<>(&ops::operator--)>;
  template struct access<overload<int>(&ops::operator--)>;
  template struct access<overload<const ops&>(&ops::operator+)>;
  template struct access<overload<const ops&>(&ops::operator-)>;
  template struct access<overload<const ops&>(&ops::operator*)>;
  template struct access<&ops::operator/>;
  template struct access<(&ops::operator%)>;
  template struct access<&ops::operator^>;
  template struct access<overload<const ops&>(&ops::operator&)>;
  template struct access<&ops::operator|>;
  template struct access<&ops::operator=>;
  template struct access<&ops::operator<>;
  template struct access<(&ops::operator>)>;
  template struct access<&ops::operator+=>;
  template struct access<&ops::operator-=>;
  template struct access<&ops::operator*=>;
  template struct access<&ops::operator/=>;
  template struct access<&ops::operator%=>;
  template struct access<&ops::operator^=>;
  template struct access<&ops::operator&=>;
  template struct access<&ops::operator|=>;
  template struct access<&ops::operator<<>;
  template struct access<(&ops::operator>>)>;
  template struct access<(&ops::operator>>=)>;
  template struct access<&ops::operator<<=>;
  template struct access<&ops::operator==>;
  template struct access<&ops::operator!=>;
  template struct access<(&ops::operator<=)>;
  template struct access<(&ops::operator>=)>;
  template struct access<(&ops::operator<=>)>;
  template struct access<&ops::operator&&>;
  template struct access<&ops::operator||>;
  template struct access<&ops::operator,>;
  template struct access<(&ops::operator->*)>;
  template struct access<&ops::operator()>;
  template struct access<&ops::operator[]>;
  template struct access<(&ops::operator->)>;

  template struct access<&ops::operator A>;
  template struct access<&ops::operator A, void, "conv_to_a">;
  // template struct access<&ops::operator A&&>;
  template struct access<&ops::operator A&&, void, "conv_to_a&&">;

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
  // same as: template struct access<man_overload<acc::const_, int>(&A::z)>;

  template struct access<man_overload<acc::volatile_, int>(&A::z)>;
  template struct access<man_overload<acc::volatile_ + acc::const_, int>(&A::z)>;

  template struct access<&A::print>;

  // other class with same name
  template struct access<&B::z>;

  // private type
  template struct type_access<A::X, A>;
  // same with rename
  template struct type_access<A::X, A, "MYX">;
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

  template struct access<&A::print_stat, A>;
#else
  // static member function
  call_static_function_with(B, cica);

  // overloaded static member function
  call_static_function_with(B, y, double);
  call_static_function_with(B, y, int, float);

  call_static_function_with(A, print_stat, const char*, const char*, int);
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
  // same with rename
  template struct access<constructing_at(C), C, "MYC">;

  template struct access<constructing_at(C, const C&), C>;
  template struct access<destructing_at(C), C>;
  // same with rename
  template struct access<destructing_at(C), C, "MYC">;

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

#if defined(__clang__)
  template struct access<private_base(B, A)>;
  template struct access<private_base(B, A), void, "renamed">;
#else
  lambda_member_accessor(decltype(lambda), a);
  lambda_member_accessor(decltype(lambda), b);
#endif

  constexpr decltype(auto) call(accessor_t<"y", B>, int, float);
}
int main() {
  using namespace access_private;

  A a;
  auto& aa = accessor<"a">(&a);

  auto c1 = accessor<"x">(a, 1, 4.3f);

  // accessor<"x">(a, 1, 4.3);
  // not work because 2nd parameter must be match with function argument type (double != float)
  // if you want to make it work, you need to define the exact parameters inside the access_private namespace:
  // constexpr decltype(auto) call(accessor_t<"x">, A&, int, float);
  // as caveat with this, you lose the noexcept specifier.

  auto vv = accessor<"x", A&, int, float>(a, 1, 4.3);
  // or you can call with double if you specify the parameters at the accessor
  // (MSVC will generate warning here because of double->float implicit conversion.)

  accessor<"y">(a, 0.0);

  auto c2 = accessor<"y">(std::as_const(a), 1, 4.3f);

  auto c3 = accessor<"z">(std::move(a), 1);
  auto c4 = accessor<"z">(std::as_const(a), 1);

  static_assert(noexcept(accessor<"z">(std::add_pointer_t<volatile A>{nullptr}, 0)));
  static_assert(!noexcept(accessor<"z">(std::add_pointer_t<const volatile A>{nullptr}, 0)));

  using XT = type_accessor<"X", A>;
  using XT2 = type_accessor<"MYX", A>;
  static_assert(std::is_same_v<XT, XT2>);

  XT copy_d = accessor<"d">(a);

  accessor<"print", A&, const char*>(a, "%d %s\n", 4, "vararg function called");

#if HAS_STATIC_MEMBER_FUNCTION
  accessor<"print_stat", A, const char*>("%s %d\n", "Static vararg called", 5);
#else
  accessor<"print_stat", A, const char*, const char*, int>("%s %d\n", "Static vararg called", 5);
#endif

  constexpr auto& p = accessor<"LAMBDA", B>();
  p();

  accessor<"cica", B>();

  accessor<"y", B>(0.1);

  accessor<"y", B>(0, 4.3);
  // the same strategy as above, but with static function.

#if not defined(_MSC_VER)
  B X {accessor<"construct", B>()};
  B X2 = accessor<"construct", B>(1, 2.2);

  union U {
    char arr;
    C c;

    U() : arr{} {};
    ~U() {}
  };
  U u;
  U oth;

  C& c = accessor<"MYC", C>(&u.c);
  C& othC = accessor<"C", C>(&oth.c, std::as_const(c));

  // C &&anoth {accessor<"construct">(std::as_const(c))}; // this will not work, because ~C is private

  accessor<"~C", C>(othC);
  accessor<"~MYC", C>(c);

  accessor<"def">(a, 4u); // calls a.def(4) with second default argument
  accessor<"def2">(a);
  accessor<"def2">(a, 1);
  accessor<"def2">(a, 1, 2.2f);

  accessor<"def", B>(4l);
  accessor<"def2", B>();
  accessor<"def2", B>(1);
  accessor<"def2", B>(1, 2.2f);
#endif

  constexpr ops_x thisis = ops_x{};
  A &&x1{accessor<"A">(thisis)};
  A &&x2{accessor<"conv_to_a">(thisis)};
  A &&x3{accessor<"conv_to_a&&">(thisis)};
  accessor<"+">(thisis);
  accessor<"+">(thisis, thisis);
  accessor<"++">(thisis);
  accessor<"++">(thisis, 0);


#if defined(__clang__)
  A* base = accessor<"A">(&X);
  A* base2 = accessor<"renamed">(&X);
#else
  int& ac = accessor<"a">(lambda);
  int& bc = accessor<"b">(lambda);
#endif

}