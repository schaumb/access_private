#ifndef ACCESS_PRIVATE_20_OVERLOAD_H
#define ACCESS_PRIVATE_20_OVERLOAD_H

#include <type_traits>
#include <utility>

namespace access_private {

#define MEM_PTR_NOE_VAR(CV_, REF_, REF_P_, ...) \
template<class Res, class M> \
consteval auto operator()(Res (M::* t)(Ts... __VA_ARGS__) CV_ REF_) CV_ REF_P_ noexcept { return t; } \
template<class Res, class M> \
consteval auto operator()(Res (M::* t)(Ts... __VA_ARGS__) CV_ REF_ noexcept) CV_ REF_P_ noexcept { return t; }

#define MEM_PTR_CV(REF_, REF_P_, ...) \
MEM_PTR_NOE_VAR(, REF_, REF_P_, __VA_ARGS__) \
MEM_PTR_NOE_VAR(const, REF_, REF_P_, __VA_ARGS__) \
MEM_PTR_NOE_VAR(volatile, REF_, REF_P_, __VA_ARGS__) \
MEM_PTR_NOE_VAR(const volatile, REF_, REF_P_, __VA_ARGS__)

  template<bool rvalue, bool vararg, class ...Ts>
  struct overload_t;

  template<class ...Ts>
  struct overload_t<false, false, Ts...> {
    template<class Res>
    consteval auto operator()(Res (*t)(Ts...)) noexcept {
      return t;
    }

    MEM_PTR_CV(,)
    MEM_PTR_CV(&,&)
    MEM_PTR_CV(&&,&&)
  };

  template<class ...Ts>
  struct overload_t<true, false, Ts...> {
    MEM_PTR_CV(&&,)
  };

  template<class ...Ts>
  struct overload_t<false, true, Ts...> {
    template<class Res>
    consteval auto operator()(Res (*t)(Ts..., ...)) noexcept {
      return t;
    }

    MEM_PTR_CV(,,, ...)
    MEM_PTR_CV(&,&,, ...)
    MEM_PTR_CV(&&,&&,, ...)
  };

  template<class ...Ts>
  struct overload_t<true, true, Ts...> {
    MEM_PTR_CV(&&,,,...)
  };

#undef MEM_PTR_CV
#undef MEM_PTR_NOE_VAR

  enum class acc : unsigned char {
    const_ = 1,
    volatile_ = 2,
    rvalue = 4,
    vararg = 8,
  };

  consteval bool operator&(const acc &lhs, const acc &rhs) {
    using uc = unsigned char;
    return static_cast<uc>(lhs) & static_cast<uc>(rhs);
  }

  consteval acc operator+(const acc &lhs, const acc &rhs) {
    using uc = unsigned char;
    return static_cast<acc>(static_cast<uc>(lhs) | static_cast<uc>(rhs));
  }

  template<template<class> class Mod, bool v, class T>
  using apply_if = std::conditional_t<v, Mod<T>, T>;

  template<class ...Ts>
  constinit inline static overload_t<false, false, Ts...> overload{};
  template<class ...Ts>
  constinit inline static const overload_t<false, false, Ts...> const_overload{};

  template<acc c, class ...Ts>
  constinit inline static
      apply_if<std::add_volatile_t, c & acc::volatile_,
               apply_if<std::add_const_t, c & acc::const_,
                  overload_t<c & acc::rvalue, c & acc::vararg, Ts...>
                        >> man_overload{};
}

#endif //ACCESS_PRIVATE_20_OVERLOAD_H
