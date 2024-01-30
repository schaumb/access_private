#ifndef ACCESS_PRIVATE_20_OVERLOAD_H
#define ACCESS_PRIVATE_20_OVERLOAD_H

#include "access_private.hpp"

#include <type_traits>
#include <utility>
#include <functional>

namespace access_private {
  template<class ...Ts>
  struct overload_t {
    template<class Res>
    consteval auto operator()(Res (*t)(Ts...)) noexcept {
      return freeptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...)) noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) const) const noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) volatile) volatile noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) const volatile) const volatile noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) &) & noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) const &) const & noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) volatile &) volatile & noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) const volatile &) const volatile & noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) &&) && noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) const &&) const && noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) volatile &&) volatile && noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) const volatile &&) const volatile && noexcept {
      return memptr{t};
    }
  };

  template<class ...Ts>
  struct rvalue_overload_t {
    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) &&) noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) const &&) const noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) volatile &&) volatile noexcept {
      return memptr{t};
    }

    template<class Res, class M>
    consteval auto operator()(Res (M::* t)(Ts...) const volatile &&) const volatile noexcept {
      return memptr{t};
    }
  };


  template<class ...Ts>
  constinit inline static overload_t<Ts...> overload{};
  template<class ...Ts>
  constinit inline static const overload_t<Ts...> const_overload{};

  enum class acc : unsigned char {
    const_ = 1,
    volatile_ = 2,
    rvalue = 4,
    const_volatile = const_ | volatile_,
    rvalue_const = const_ | rvalue,
    rvalue_volatile = volatile_ | rvalue,
    rvalue_const_volatile = const_ | volatile_ | rvalue
  };

  consteval bool operator&(const acc &lhs, const acc &rhs) {
    using uc = unsigned char;
    return static_cast<uc>(lhs) & static_cast<uc>(rhs);
  }

  template<template<class> class Mod, bool v, class T>
  using apply_if = std::conditional_t<v, Mod<T>, T>;

  template<acc c, class ...Ts>
  constinit inline static
      apply_if<std::add_volatile_t, c & acc::volatile_,
               apply_if<std::add_const_t, c & acc::const_,
                        overload_t<Ts...>>> man_overload{};

  template<acc c, class ...Ts> requires(c & acc::rvalue)
  constinit inline static
      apply_if<std::add_volatile_t, c & acc::volatile_,
               apply_if<std::add_const_t, c & acc::const_,
                        rvalue_overload_t<Ts...>>> man_overload<c, Ts...>{};
}

#endif //ACCESS_PRIVATE_20_OVERLOAD_H
