# Introduction

This library is a collection of template hacks with which we can access private members.
Why would you need this?
Testing.
There are some cases when we want to test a class, but we can't or don't want to modify it.
The reasons behind that might be the following:
  * It is part of a third party software package and
    * Our build system would overwrite the changes we made
    * We don't want to maintain our own version
  * Touching the internals would require tremendous amount of recompilation of client codes, which might not be desired.

Why not use `#define private public`?
Because that's undefined behaviour.
The C++ standard states that the relative order of members in a class with different access specifiers is undefined.

# Usage
```c++
class A {
  int m_i = 3;
  int m_f(int p) { return 14 * p; }
};

namespace access_private {
  template struct access<&A::m_i>;
}

void foo() {
  A a;
  auto &i = access_private::accessor<"m_i">(a);
  assert(i == 3);
}

namespace access_private {
  template struct access<&A::m_f>;
}

void bar() {
  A a;
  int p = 3;
  auto res = access_private::accessor<"m_f">(a, std::move(p));
  assert(res == 42);
}
```

You can call private member functions and static private functions, overloaded functions without macro.  
You can also access static private variables without macro.  
You can invoke private constructors and destructor.  
You can invoke private member functions with they default arguments.  
For DETAILED USAGE and EXAMPLES, please take a look [test.cpp](https://github.com/schaumb/access_private/blob/master/test/test.cpp) and [new_tests.cpp](https://github.com/schaumb/access_private/blob/master/test/new_tests.cpp)!

# How does it work?
The ISO C++ standard specifies that there is no access check in case of explicit
template instantiations (C++14 / 14.7.2 para 12).
We can exploit this by defining a static pointer to member (or a friend function), which holds (returns) the address of the private member.
References:
* https://gist.github.com/dabrahams/1528856
* Advanced C++ FAQs: Volumes 1 & 2, pp 289 - 296

# Limitations

* We cannot access private members that are references. (See this [issue](https://github.com/martong/access_private/issues/12).)
* On MSVC, we cannot call private constructors / destructors, and we cannot access the default arguments of the private functions. (See this [issue](http://tinyurl.com/msvcconstructor).)
* We have a link time error in case of only in-class declared `const static` variables if used as reference/pointer (or in debug build). That's because we'd take the address of that, and if that is not defined (i.e the compiler do a compile-time insert of the const value), we'd have an undefined symbol.

# Compilers
I have done tests for the following compilers:
* LLVM version 16
* GCC
  * 13.2
  * 12.3
* MSVC

Test code is compiled with -std=c++20 .

# Notes
There is a [C++ standard *issue*](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#2118) that says: 
> Stateful metaprogramming via friend injection techniques should be ill-formed

The `::access_private::accessor_t<...>` template class implements a friend function `get()`, which is used after class definition.
I am not sure, however, if that issue has been ever fixed.
