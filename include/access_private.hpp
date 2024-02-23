#ifndef ACCESS_PRIVATE_20_LIBRARY_H
#define ACCESS_PRIVATE_20_LIBRARY_H

#include <string_view>
#include <array>
#include <functional>

namespace access_private {
  template<class Ptr, class Res, bool Noexcept = true, bool Vararg = false, class ...Args>
  struct freeptr {
    using res_type = Res;
    constexpr static bool is_noexcept = Noexcept;
    constinit static const auto args = sizeof...(Args);

    constexpr freeptr(Ptr ptr) : ptr{ptr} {}

    constexpr freeptr operator+() const { return *this; }

    Ptr ptr;
  };

  template<class Ptr, class Res, class Class, bool Noexcept = true, bool Vararg = false, class ...Args>
  struct memptr {
    constexpr static bool is_noexcept = Noexcept;
    constexpr memptr(Ptr ptr) : ptr{ptr} {}

    constexpr memptr operator+() const { return *this; }

    Ptr ptr;
  };

  template<class Res, class ...Args>
  freeptr(Res (*)(Args...)) -> freeptr<Res(*)(Args...), Res, false, false, Args...>;
  template<class Res, class ...Args>
  freeptr(Res (*)(Args...) noexcept) -> freeptr<Res(*)(Args...), Res, true, false, Args...>;
  template<class Res, class ...Args>
  freeptr(Res (*)(Args..., ...)) -> freeptr<Res(*)(Args...), Res, false, true, Args...>;
  template<class Res, class ...Args>
  freeptr(Res (*)(Args..., ...) noexcept) -> freeptr<Res(*)(Args...), Res, true, true, Args...>;

  template<class Res>
  freeptr(Res *) -> freeptr<Res *, Res>;

  template<class Res, class M, class ...Ts>
  memptr(Res (* t)(M*, Ts...)) -> memptr<decltype(t), Res, M *, false, false, Ts...>;
  template<class Res, class M, class ...Ts>
  memptr(Res (* t)(M*, Ts...) noexcept) -> memptr<decltype(t), Res, M *, true, false, Ts...>;
  template<class Res, class M, class ...Ts>
  memptr(Res (* t)(M*, Ts..., ...)) -> memptr<decltype(t), Res, M *, false, true, Ts...>;
  template<class Res, class M, class ...Ts>
  memptr(Res (* t)(M*, Ts..., ...) noexcept) -> memptr<decltype(t), Res, M *, true, true, Ts...>;

  template<class Res, class M>
  memptr(Res M::* t) -> memptr<decltype(t), Res, M>;

#define MEM_PTR_GETTER_NOE_VAR(CV_, REF_, REF_P_) \
  template<class Res, class M, class ...Ts> \
  memptr(Res (M::* t)(Ts...     ) CV_ REF_         ) -> memptr<decltype(t), Res, CV_ M REF_P_, false, false, Ts...>; \
  template<class Res, class M, class ...Ts> \
  memptr(Res (M::* t)(Ts...     ) CV_ REF_ noexcept) -> memptr<decltype(t), Res, CV_ M REF_P_, true , false, Ts...>; \
  template<class Res, class M, class ...Ts> \
  memptr(Res (M::* t)(Ts..., ...) CV_ REF_         ) -> memptr<decltype(t), Res, CV_ M REF_P_, false, true , Ts...>; \
  template<class Res, class M, class ...Ts> \
  memptr(Res (M::* t)(Ts..., ...) CV_ REF_ noexcept) -> memptr<decltype(t), Res, CV_ M REF_P_, true , true , Ts...>;

#define MEM_PTR_GETTER_CV(REF_, REF_P_) \
  MEM_PTR_GETTER_NOE_VAR(		       , REF_, REF_P_) \
  MEM_PTR_GETTER_NOE_VAR(const		   , REF_, REF_P_) \
  MEM_PTR_GETTER_NOE_VAR(volatile	   , REF_, REF_P_) \
  MEM_PTR_GETTER_NOE_VAR(const volatile, REF_, REF_P_)

MEM_PTR_GETTER_CV(  , *)
MEM_PTR_GETTER_CV( &, &)
MEM_PTR_GETTER_CV(&&,&&)

#undef MEM_PTR_GETTER_CV
#undef MEM_PTR_GETTER_NOE_VAR

#ifdef _MSC_VER
  template <class T> struct wrap {
    static T not_exists;
  };

  template <class Base, class Member>
  static Base getBase(Member Base::*);
#endif

  template<class T, auto ptr>
  consteval auto name_impl() {
#ifdef _MSC_VER
    if constexpr (std::is_member_object_pointer_v<decltype(ptr)>) {
      return name_impl<void,
                       &std::invoke(ptr,
                                    wrap<decltype(getBase(ptr))>::not_exists)>();
    } else {

      constexpr std::string_view sv = __FUNCSIG__;
      constexpr auto last = [] (std::string_view sv, std::size_t ix) {
        while (true) {
          switch(sv[ix]) {
          case ' ':
          case '&':
            --ix; break;
          case ')':
            for (std::size_t brackets = 1;
                 brackets > 0;) {
              switch (sv[--ix]) {
              case ')':
                ++brackets; break;
              case '(':
                if (--brackets == 0)
                  --ix;
                break;
              }
            }
            break;
          case '>':
            for (std::size_t brackets = 1;
                 brackets > 0;) {
              switch (sv[--ix]) {
              case '>':
                ++brackets; break;
              case '<':
                if (--brackets == 0)
                  --ix;
                break;
              }
            }
          case 't':
            if (sv.substr(0, ix).ends_with(" cons"))
              ix-=5;
            else if (sv.substr(0, ix).ends_with(" noexcep"))
              ix-=8;
            else
              return ix;
            break;
          case 'e':
            if (sv.substr(0, ix).ends_with(" volatil"))
              ix-=8;
            else
              return ix;
            break;
          default:
            return ix;
          }
        }
        return ix;
      } (sv, sv.find_last_not_of(" >(}", sv.size() - 6));

#else
    constexpr std::string_view sv = __PRETTY_FUNCTION__;
    constexpr auto last = sv.find_last_not_of(" ])}");
#endif
      constexpr auto first =
          sv.find_last_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789", last);
      std::array<char, last - first + 1> res{};
      auto it = res.data();
      for (auto a = first + 1; a <= last; ++a)
        *it++ = sv[a];
      return res;

#ifdef _MSC_VER
    }
#endif
  }

  template<typename T>
  consteval auto type_name() {
#ifdef _MSC_VER
    constexpr std::string_view sv = __FUNCSIG__;
    constexpr auto last = sv.find_last_not_of(" >(", sv.size() - 6);
#else
    constexpr std::string_view sv = __PRETTY_FUNCTION__;
    constexpr auto last = sv.find_last_not_of(" ])}");
#endif
    constexpr auto first =
        sv.find_last_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789", last);
    std::array<char, last - first + 1> res{};
    auto it = res.data();
    for (auto a = first + 1; a <= last; ++a)
      *it++ = sv[a];
    return res;
  }

  template<std::size_t N>
  struct static_string {
    consteval static_string(const char(&c)[N]) noexcept {
      for (auto *p = arr; auto ch: c)
        *p++ = ch;
    }

    consteval static_string(std::array<char, N> c) noexcept {
      for (auto *p = arr; auto ch: c)
        *p++ = ch;
    }

    consteval static_string(char prefix, std::array<char, N - 1> c) noexcept {
      auto *p = arr;
      *p++ = prefix;
      for (auto ch: c)
        *p++ = ch;
    }

    char arr[N]{};
  };

  template<std::size_t N>
  static_string(char prefix, std::array<char, N> c) -> static_string<N + 1>;

  template<static_string S, class ...Args>
  struct accessor_t;

  template<static_string S>
  struct accessor_t<S> {
    template<class First, class ...Ts>
    constexpr auto operator()(First &&f, Ts &&... ts) const
      noexcept(memptr{accessor_t<S>{}.ptr<std::conditional_t<
                                      std::is_pointer_v<First>,
                                      First,
                                      First &&>, Ts...>()}.is_noexcept)
        -> decltype((get(accessor_t<S,
                                    std::conditional_t<
                                        std::is_pointer_v<First>,
                                        First,
                                        First &&>, Ts...>{}, std::forward<First>(f), std::forward<Ts>(ts)...))) {
      return (get(accessor_t<S,
                             std::conditional_t<
                                 std::is_pointer_v<First>,
                                 First,
                                 First &&>, Ts...>{}, std::forward<First>(f), std::forward<Ts>(ts)...));
    }

    template<class Base, class ...Ts>
    constexpr auto on_type(Ts &&... ts) const
      noexcept(freeptr{accessor_t<S>{}.static_ptr<Base, Ts...>()}.is_noexcept)
        -> decltype((get(accessor_t<S, std::type_identity<Base> *, Ts...>{},
                         static_cast<std::type_identity<Base> *>(nullptr), std::forward<Ts>(ts)...))) {
      return (get(accessor_t<S, std::type_identity<Base> *, Ts...>{},
                  static_cast<std::type_identity<Base> *>(nullptr), std::forward<Ts>(ts)...));
    }

    template<class T> requires(requires{ get(accessor_t<S, T *>{}, static_cast<T *>(nullptr)); })
    using type =
        std::remove_pointer_t<decltype(get(accessor_t<S, T *>{}, static_cast<T *>(nullptr)))>;

    template<class ...Ts>
    consteval auto ptr() const
        -> decltype(get(accessor_t<S, Ts...>{})) {
      return get(accessor_t<S, Ts...>{});
    }

    template<class Base, class ...Ts>
    consteval auto static_ptr() const
        -> decltype(get(accessor_t<S, std::type_identity<Base> *, Ts...>{})) {
      return get(accessor_t<S, std::type_identity<Base> *, Ts...>{});
    }
  };

#if not defined(__clang__) and defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wnon-template-friend"
#endif

  template<static_string S, class A, class ...Args>
  struct accessor_t<S, A, Args...> : accessor_t<S> {
    friend constexpr auto get(accessor_t<S, A, Args...>);

    friend constexpr decltype(auto) get(accessor_t<S>, A, Args ...);

    template<class First, class ...Ts>
    constexpr decltype(auto) operator()(First &&f, Ts &&... ts) const
      noexcept(memptr{get(accessor_t<S, A, Args...>{})}.is_noexcept)
    {
      return (get(accessor_t<S, A, Args...>{}, std::forward<First>(f), std::forward<Ts>(ts)...));
    }

    template<class Base, class ...Ts>
    constexpr decltype(auto) call(Ts &&... ts) const
      noexcept(freeptr{get(accessor_t<S, A, Args...>{})}.is_noexcept)
    {
      return (get(accessor_t<S, std::type_identity<Base> *, Ts...>{},
                  static_cast<std::type_identity<Base> *>(nullptr), std::forward<Ts>(ts)...));
    }
  };

#if not defined(__clang__) and defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif


  template<auto T, class = void, class Type = decltype(T),
            auto str = name_impl<decltype(T.ptr), T.ptr>(),
            bool workaround = false
            >
  struct access_impl;

  template<auto T, class Ptr, class Res, class Class, bool noexc, bool varg, class ...Args,
            auto str> requires(std::is_reference_v<Class>)
  struct access_impl<T, void, memptr<Ptr, Res, Class, noexc, varg, Args...>, str, false> :
      accessor_t<str, Class, Args...> {
    using super_base = accessor_t<str>;
    using base = accessor_t<str, Class, Args...>;

    friend constexpr auto get(base) {
      return T.ptr;
    }

    friend constexpr decltype(auto) get(super_base, Class this_, Args... args) {
      return (std::invoke(T.ptr, std::forward<Class>(this_), std::forward<Args>(args)...));
    }
  };

  template<auto T, class Ptr, class Res, class Class, bool noexc, bool varg, class ...Args,
            auto str> requires(std::is_pointer_v<Class>)
  struct access_impl<T, void, memptr<Ptr, Res, Class, noexc, varg, Args...>, str, false> :
      accessor_t<str, Class, Args...>,
      accessor_t<str, std::remove_pointer_t<Class> &, Args...>,
      accessor_t<str, std::remove_pointer_t<Class> &&, Args...> {
    using rptr = std::remove_pointer_t<Class>;
    using super_base = accessor_t<str>;
    using base_1 = accessor_t<str, Class, Args...>;
    using base_2 = accessor_t<str, rptr &, Args...>;
    using base_3 = accessor_t<str, rptr &&, Args...>;

    friend constexpr auto get(base_1) { return memptr{+T}.ptr; }

    friend constexpr auto get(base_2) { return memptr{+T}.ptr; }

    friend constexpr auto get(base_3) { return memptr{+T}.ptr; }

    friend constexpr decltype(auto) get(super_base, Class this_, Args... args) {
      return (std::invoke(memptr{+T}.ptr, this_, std::forward<Args>(args)...));
    }

    friend constexpr decltype(auto) get(super_base, rptr &this_, Args... args) {
      return (std::invoke(memptr{+T}.ptr, &this_, std::forward<Args>(args)...));
    }

    friend constexpr decltype(auto) get(super_base, rptr &&this_, Args... args) {
      return (std::invoke(memptr{+T}.ptr, &this_, std::forward<Args>(args)...));
    }
  };


  template<auto T, class Ptr, class Res, class Class,
            auto str> requires(!std::is_pointer_v<Class> && !std::is_reference_v<Class>)
  struct access_impl<T, void, memptr<Ptr, Res, Class, true, false>, str, false> :
      accessor_t<str, Class *>,
      accessor_t<str, const Class *>,
      accessor_t<str, volatile Class *>,
      accessor_t<str, const volatile Class *>,
      accessor_t<str, Class &>,
      accessor_t<str, const Class &>,
      accessor_t<str, volatile Class &>,
      accessor_t<str, const volatile Class &>,
      accessor_t<str, Class &&>,
      accessor_t<str, const Class &&>,
      accessor_t<str, volatile Class &&>,
      accessor_t<str, const volatile Class &&> {
    using super_base = accessor_t<str>;
    using base_1 = accessor_t<str, Class *>;
    using base_2 = accessor_t<str, const Class *>;
    using base_3 = accessor_t<str, volatile Class *>;
    using base_4 = accessor_t<str, const volatile Class *>;
    using base_5 = accessor_t<str, Class &>;
    using base_6 = accessor_t<str, const Class &>;
    using base_7 = accessor_t<str, volatile Class &>;
    using base_8 = accessor_t<str, const volatile Class &>;
    using base_9 = accessor_t<str, Class &&>;
    using base_a = accessor_t<str, const Class &&>;
    using base_b = accessor_t<str, volatile Class &&>;
    using base_c = accessor_t<str, const volatile Class &&>;

    friend constexpr auto get(base_1) { return T.ptr; }

    friend constexpr auto get(base_2) { return T.ptr; }

    friend constexpr auto get(base_3) { return T.ptr; }

    friend constexpr auto get(base_4) { return T.ptr; }

    friend constexpr auto get(base_5) { return T.ptr; }

    friend constexpr auto get(base_6) { return T.ptr; }

    friend constexpr auto get(base_7) { return T.ptr; }

    friend constexpr auto get(base_8) { return T.ptr; }

    friend constexpr auto get(base_9) { return T.ptr; }

    friend constexpr auto get(base_a) { return T.ptr; }

    friend constexpr auto get(base_b) { return T.ptr; }

    friend constexpr auto get(base_c) { return T.ptr; }

    friend constexpr decltype(auto) get(super_base, Class *this_) { return (std::invoke(T.ptr, this_)); }

    friend constexpr decltype(auto) get(super_base, const Class *this_) { return (std::invoke(T.ptr, this_)); }

    friend constexpr decltype(auto) get(super_base, volatile Class *this_) { return (std::invoke(T.ptr, this_)); }

    friend constexpr decltype(auto) get(super_base, const volatile Class *this_) {
      return (std::invoke(T.ptr, this_));
    }

    friend constexpr decltype(auto) get(super_base, Class &this_) { return (std::invoke(T.ptr, this_)); }

    friend constexpr decltype(auto) get(super_base, const Class &this_) { return (std::invoke(T.ptr, this_)); }

    friend constexpr decltype(auto) get(super_base, volatile Class &this_) { return (std::invoke(T.ptr, this_)); }

    friend constexpr decltype(auto) get(super_base, const volatile Class &this_) {
      return (std::invoke(T.ptr, this_));
    }

    friend constexpr decltype(auto) get(super_base, Class &&this_) {
      return (std::invoke(T.ptr, std::move(this_)));
    }

    friend constexpr decltype(auto) get(super_base, const Class &&this_) {
      return (std::invoke(T.ptr, std::move(this_)));
    }

    friend constexpr decltype(auto) get(super_base, volatile Class &&this_) {
      return (std::invoke(T.ptr, std::move(this_)));
    }

    friend constexpr decltype(auto) get(super_base, const volatile Class &&this_) {
      return (std::invoke(T.ptr, std::move(this_)));
    }
  };


  template<auto T, class Base, class Ptr, class Res, bool noexc, bool varg, class ...Args,
            auto str>
    requires(!std::is_void_v<Base>)
  struct access_impl<T, Base, freeptr<Ptr, Res, noexc, varg, Args...>, str, false> :
      accessor_t<str, std::type_identity<Base> *, Args...> {
    using super_base = accessor_t<str>;
    using base = accessor_t<str, std::type_identity<Base> *, Args...>;

    friend constexpr auto get(base) {
      return freeptr{+T}.ptr;
    }

    friend constexpr decltype(auto) get(super_base, std::type_identity<Base> *this_, Args... args) {
      if constexpr (std::is_function_v<std::remove_pointer_t<Ptr>>) {
        return (std::invoke(freeptr{+T}.ptr, std::forward<Args>(args)...));
      } else {
        return *freeptr{+T}.ptr;
      }
    }
  };

  template<auto T, class Base, class Ptr, class Res, bool noexc, bool varg, class ...Args,
            auto str>
  struct access_impl<T, Base, freeptr<Ptr, Res, noexc, varg, std::tuple<Args...> &&>, str, true> :
      accessor_t<str, std::type_identity<Base> *, Args...> {
    using super_base = accessor_t<str>;
    using base = accessor_t<str, std::type_identity<Base> *, Args...>;

    friend constexpr auto get(base) {
      return freeptr{+T}.ptr;
    }

    friend constexpr decltype(auto) get(super_base, std::type_identity<Base> *this_, Args... args) {
      return (std::invoke(freeptr{+T}.ptr, std::forward_as_tuple(std::forward<Args>(args)...)));
    }
  };


  template<auto T, class Ptr, class Res, class Class, bool noexc, bool varg, class ...Args,
            auto str> requires(std::is_pointer_v<Class>)
  struct access_impl<T, void, memptr<Ptr, Res, Class, noexc, varg, std::tuple<Args...> &&>, str, true> :
      accessor_t<str, Class, Args...>,
      accessor_t<str, std::remove_pointer_t<Class> &, Args...>,
      accessor_t<str, std::remove_pointer_t<Class> &&, Args...> {
    using rptr = std::remove_pointer_t<Class>;
    using super_base = accessor_t<str>;
    using base_1 = accessor_t<str, Class, Args...>;
    using base_2 = accessor_t<str, rptr &, Args...>;
    using base_3 = accessor_t<str, rptr &&, Args...>;

    friend constexpr auto get(base_1) { return memptr{+T}.ptr; }

    friend constexpr auto get(base_2) { return memptr{+T}.ptr; }

    friend constexpr auto get(base_3) { return memptr{+T}.ptr; }

    friend constexpr decltype(auto) get(super_base, Class this_, Args... args) {
      return (std::invoke(memptr{+T}.ptr, this_, std::forward_as_tuple(std::forward<Args>(args)...)));
    }

    friend constexpr decltype(auto) get(super_base, rptr &this_, Args... args) {
      return (std::invoke(memptr{+T}.ptr, &this_, std::forward_as_tuple(std::forward<Args>(args)...)));
    }

    friend constexpr decltype(auto) get(super_base, rptr &&this_, Args... args) {
      return (std::invoke(memptr{+T}.ptr, &this_, std::forward_as_tuple(std::forward<Args>(args)...)));
    }
  };

  template<auto T, class Base, class Ptr, class Res, bool noexc, bool varg, class ...Args,
            auto str>
  struct access_impl<T, Base, freeptr<Ptr, Res, noexc, varg, std::remove_reference_t<Res> *, std::tuple<Args...> &&>, str, true> :
      accessor_t<str, std::type_identity<Base> *, std::remove_reference_t<Res> *, Args...> {
    using super_base = accessor_t<str>;
    using base = accessor_t<str, std::type_identity<Base> *, std::remove_reference_t<Res> *, Args...>;

    friend constexpr auto get(base) {
      return freeptr{+T}.ptr;
    }

    friend constexpr decltype(auto)
    get(super_base, std::type_identity<Base> *this_, std::remove_reference_t<Res> *pure_this, Args... args) {
      return (std::invoke(freeptr{+T}.ptr, pure_this, std::forward_as_tuple(std::forward<Args>(args)...)));
    }
  };

  template<auto T, class = void>
  struct access;

  template<auto T> requires(requires { memptr{T}; })
  struct access<T, void> : access_impl<memptr{T}> {
  };

  template<auto T, class V> requires(std::same_as<decltype(T), decltype(+T)> && requires { freeptr{T}; } &&
             !std::is_void_v<V>)
  struct access<T, V> : access_impl<freeptr{T}, V> {
  };

  template<auto T, class V> requires(!std::same_as<decltype(T), decltype(+T)> &&
             !std::is_void_v<V> &&
             std::same_as<V, typename decltype(freeptr{+T})::res_type>)
  struct access<T, V> : access_impl<T, V, decltype(freeptr{+T}),
                                    static_string{"construct"}
#if defined(__clang__)
                                    , 0 < decltype(freeptr{+T})::args
#endif
                                    > {
  };

  template<auto T, class V> requires(!std::same_as<decltype(T), decltype(+T)> && !std::is_void_v<V> &&
             std::is_void_v<typename decltype(freeptr{+T})::res_type>)
  struct access<T, V> : access_impl<T, V, decltype(freeptr{+T}),
                                    static_string{'~', type_name<V>()}
                                    > {
  };

  template<auto T, class V> requires(!std::same_as<decltype(T), decltype(+T)> && !std::is_void_v<V> &&
             std::same_as<V &, typename decltype(freeptr{+T})::res_type>)
  struct access<T, V> : access_impl<T, V, decltype(freeptr{+T}),
                                    type_name<V>()
#if defined(__clang__)
                                        , 1 < decltype(freeptr{+T})::args
#endif
                                    > {
  };

  template<auto T> requires(!std::same_as<decltype(T), decltype(+T)> &&
             std::is_pointer_v<typename decltype(freeptr{+T})::res_type>)
  struct access<T, void> : access_impl<T, void, decltype(memptr{+T}),
                                   type_name<std::remove_pointer_t<typename decltype(freeptr{+T})::res_type>>()
                                    > {
  };

  template<auto T, static_string S, class V = void>
  struct unique_access;
  template<auto T, static_string S>
    requires(std::is_member_object_pointer_v<decltype(T)>)
  struct unique_access<T, S, void> : access_impl<memptr{T}, void,
                                                 decltype(memptr{T}), S
                                                 > {
  };

  template<auto T, static_string S>
    requires(!std::same_as<decltype(T), decltype(+T)>)
  struct unique_access<T, S, void> : access_impl<T, void,
                                                 decltype(memptr{+T}), S
#if defined(__clang__)
                                                 , 1 < decltype(freeptr{+T})::args
#endif
                                                 > {
  };

  template<auto T, static_string S, class V>
    requires(!std::same_as<decltype(T), decltype(+T)> && !std::is_void_v<V>)
  struct unique_access<T, S, V> : access_impl<T, V, decltype(freeptr{+T}), S
#if defined(__clang__)
                                              , 0 < decltype(freeptr{+T})::args
#endif
                                              > {
  };

  template<static_string S, class ...Args>
  constexpr static accessor_t<S, Args...> accessor{};

  template<class Base, class T>
  struct type_access
      : accessor_t<type_name<T>(), Base *> {
    friend constexpr decltype(auto) get(accessor_t<type_name<T>()>, Base *) {
      return static_cast<T *>(nullptr);
    }
  };

  template<class Base, static_string S>
  using type_accessor = typename accessor_t<S>::template type<Base>;


#if defined(__clang__)
#define ACCESS_PRIVATE_GET_TUPLE_ARGE(...)
#define ACCESS_PRIVATE_GET_TUPLE_ARG0(x) std::forward<std::tuple_element_t<0, std::remove_reference_t<decltype(t)>>>(std::get<0>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG1(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG0(__VA_ARGS__), std::forward<std::tuple_element_t<1, std::remove_reference_t<decltype(t)>>>(std::get<1>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG2(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG1(__VA_ARGS__), std::forward<std::tuple_element_t<2, std::remove_reference_t<decltype(t)>>>(std::get<2>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG3(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG2(__VA_ARGS__), std::forward<std::tuple_element_t<3, std::remove_reference_t<decltype(t)>>>(std::get<3>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG4(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG3(__VA_ARGS__), std::forward<std::tuple_element_t<4, std::remove_reference_t<decltype(t)>>>(std::get<4>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG5(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG4(__VA_ARGS__), std::forward<std::tuple_element_t<5, std::remove_reference_t<decltype(t)>>>(std::get<5>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG6(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG5(__VA_ARGS__), std::forward<std::tuple_element_t<6, std::remove_reference_t<decltype(t)>>>(std::get<6>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG7(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG6(__VA_ARGS__), std::forward<std::tuple_element_t<7, std::remove_reference_t<decltype(t)>>>(std::get<7>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG8(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG7(__VA_ARGS__), std::forward<std::tuple_element_t<8, std::remove_reference_t<decltype(t)>>>(std::get<8>(t))
#define ACCESS_PRIVATE_GET_TUPLE_ARG9(x, ...) ACCESS_PRIVATE_GET_TUPLE_ARG8(__VA_ARGS__), std::forward<std::tuple_element_t<9, std::remove_reference_t<decltype(t)>>>(std::get<9>(t))

#define ACCESS_PRIVATE_GET_TUPLE_ARG(i, ...) ACCESS_PRIVATE_GET_TUPLE_ARGX(i, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_TUPLE_ARGX(i, ...) ACCESS_PRIVATE_GET_TUPLE_ARG##i(__VA_ARGS__)

#define ACCESS_PRIVATE_VA_NUM_ARGS(...) ACCESS_PRIVATE_VA_NUM_ARGS_IMPL(__VA_ARGS__ __VA_OPT__(,) 9,8,7,6,5,4,3,2,1,0,E)
#define ACCESS_PRIVATE_VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N

#define constructor(T, ...) [] (std::tuple<__VA_ARGS__>&& t) \
  noexcept(std::is_nothrow_constructible_v<T __VA_OPT__(,) __VA_ARGS__>) {                                          \
    return T{ACCESS_PRIVATE_GET_TUPLE_ARG(ACCESS_PRIVATE_VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)}; \
}

#define constructing_at(T, ...) [] (T* p, std::tuple<__VA_ARGS__>&& t) \
  noexcept(std::is_nothrow_constructible_v<T __VA_OPT__(,) __VA_ARGS__>) -> T& {                                         \
    return *new (p) T{ACCESS_PRIVATE_GET_TUPLE_ARG(ACCESS_PRIVATE_VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)};    \
}
#define destructing_at(T) [] (T& p) noexcept(std::is_nothrow_destructible_v<T>) { p.~T(); }

#define call_member_function_with(T, member, ...) \
template struct unique_access<[] (T* p, std::tuple<__VA_ARGS__>&& t)\
  noexcept(noexcept(p->member(ACCESS_PRIVATE_GET_TUPLE_ARG(ACCESS_PRIVATE_VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__))))\
  -> decltype(auto) {         \
    return p->member(ACCESS_PRIVATE_GET_TUPLE_ARG(ACCESS_PRIVATE_VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__));  \
}, #member>

#define call_static_function_with(T, member, ...) \
template struct unique_access<[] (std::tuple<__VA_ARGS__>&& t) \
  noexcept(noexcept(T::member(ACCESS_PRIVATE_GET_TUPLE_ARG(ACCESS_PRIVATE_VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__))))\
  -> decltype(auto) {         \
    return T::member(ACCESS_PRIVATE_GET_TUPLE_ARG(ACCESS_PRIVATE_VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__));  \
}, #member, T>

#define private_base(Derived, Base) [](Derived* p) noexcept -> Base* { return p; }

#define lambda_member_accessor(LType, Member) \
   [] { static_assert(false, "Lambda member access is not supported in clang"); }

#elif not defined(_MSC_VER)
  template<class T, class ...Ts>
  constexpr inline static auto constructor_ = [] (Ts...ts) noexcept(std::is_nothrow_constructible_v<T, Ts...>) -> T { return T{std::forward<Ts>(ts)...}; };
  template<class T, class ...Ts>
  constexpr inline static auto construct_at_ = [] (T* p, Ts...ts)  noexcept(std::is_nothrow_constructible_v<T, Ts...>) -> T& { return *new (p) T{std::forward<Ts>(ts)...}; };
  template<class T>
  constexpr inline static auto destruct_at_ = [] (T& p)  noexcept(std::is_nothrow_destructible_v<T>) { p.~T(); };

  template<class F, static_string S, class Args, class T, class ...Ts>
  constexpr inline static auto call_member_ = []{};

  template<class Derived, class Base>
  constexpr inline static auto cast_base_ = [] (Derived* p) noexcept -> Base* {
    static_assert(!std::is_same_v<Derived, Derived>, "Private base class access is not supported in gcc. See at https://tinyurl.com/gccprivatebaseclass");
    return p;
  };

#define constructor(T, ...) ::access_private::constructor_<T __VA_OPT__(,) __VA_ARGS__>
#define constructing_at(T, ...) ::access_private::construct_at_<T __VA_OPT__(,) __VA_ARGS__>
#define destructing_at(T) ::access_private::destruct_at_<T>

#define call_member_function_with(T, member, ...) \
    template<class X, class ...Ts> \
    constexpr inline static auto call_member_<T, #member, std::tuple<__VA_ARGS__>, X, Ts...> = [] (X* p, Ts...ts) \
      noexcept(noexcept(p->member(std::forward<Ts>(ts)...))) -> decltype(auto) { \
        return p->member(std::forward<Ts>(ts)...); \
    };                                            \
    template struct unique_access<call_member_<T, #member, std::tuple<__VA_ARGS__>, T __VA_OPT__(,) __VA_ARGS__>, #member>
                                                  \
#define call_static_function_with(T, member, ...) \
    template<class X, class ...Ts> \
    constexpr inline static auto call_member_<T, #member, std::tuple<__VA_ARGS__>, X, Ts...> = [] (Ts...ts) \
      noexcept(noexcept(X::member(std::forward<Ts>(ts)...))) -> decltype(auto) { \
        return X::member(std::forward<Ts>(ts)...); \
    };                                            \
    template struct unique_access<call_member_<T, #member, std::tuple<__VA_ARGS__>, T __VA_OPT__(,) __VA_ARGS__>, #member, T>\

#define private_base(Derived, Base) ::access_private::cast_base_<Derived, Base>

#define lambda_member_accessor(LType, Member) \
   template struct unique_access<&LType::__ ## Member, #Member>

#else
#define constructor(T, ...) [] { static_assert(false, "Private constructor access is not supported in visual studio. Please upvote https://tinyurl.com/msvcconstructor"); }
#define constructing_at(T, ...) [] { static_assert(false, "Private constructor access is not supported in visual studio. Please upvote https://tinyurl.com/msvcconstructor"); }
#define destructing_at(T) [] { static_assert(false, "Private destructor access is not supported in visual studio. Please upvote https://tinyurl.com/msvcconstructor"); }
#define call_member_function_with(T, member, ...) [] { static_assert(false, "Unique member function access is not supported in visual studio. Please upvote https://tinyurl.com/msvcconstructor"); }
#define call_static_function_with(T, member, ...) [] { static_assert(false, "Unique static function access is not supported in visual studio. Please upvote https://tinyurl.com/msvcconstructor"); }
#define private_base(Derived, Base) [] { static_assert(false, "Private base class access is not supported in visual studio. Please upvote https://tinyurl.com/msvcconstructor"); }

#define lambda_member_accessor(LType, Member) \
    template struct access<&LType::Member>

#endif

}

#endif //ACCESS_PRIVATE_20_LIBRARY_H
