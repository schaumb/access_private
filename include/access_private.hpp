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
    constexpr static bool has_vararg = Vararg;
    constexpr static auto args = sizeof...(Args);

    constexpr freeptr(Ptr ptr) : ptr{ptr} {}

    constexpr freeptr operator+() const { return *this; }

    Ptr ptr;
  };

  template<class Ptr, class Res, class Class, bool Noexcept = true, bool Vararg = false, class ...Args>
  struct memptr {
    using res_type = Res;
    using class_type = Class;
    constexpr static bool is_noexcept = Noexcept;
    constexpr static bool has_vararg = Vararg;
    constexpr static auto args = sizeof...(Args);

    constexpr memptr(Ptr ptr) : ptr{ptr} {}

    constexpr memptr operator+() const { return *this; }

    Ptr ptr;
  };

  template<class Res, class ...Args>
  freeptr(Res (* t)(Args...)) -> freeptr<decltype(t), Res, false, false, Args...>;
  template<class Res, class ...Args>
  freeptr(Res (* t)(Args...) noexcept) -> freeptr<decltype(t), Res, true, false, Args...>;
  template<class Res, class ...Args>
  freeptr(Res (* t)(Args..., ...)) -> freeptr<decltype(t), Res, false, true, Args...>;
  template<class Res, class ...Args>
  freeptr(Res (* t)(Args..., ...) noexcept) -> freeptr<decltype(t), Res, true, true, Args...>;

  template<class Res>
    requires(!std::is_function_v<Res>)
  freeptr(Res *) -> freeptr<Res *, Res>;

#define MEM_PTR_FROM_FREEPTR_NOE_VAR(MOD) \
  template<class Res, class M, class ...Ts> \
    requires(std::is_class_v<M>) \
  memptr(Res (* t)(M MOD, Ts...)) -> memptr<decltype(t), Res, M MOD, false, false, Ts...>; \
  template<class Res, class M, class ...Ts> \
    requires(std::is_class_v<M>) \
  memptr(Res (* t)(M MOD, Ts...) noexcept) -> memptr<decltype(t), Res, M MOD, true, false, Ts...>; \
  template<class Res, class M, class ...Ts> \
    requires(std::is_class_v<M>) \
  memptr(Res (* t)(M MOD, Ts..., ...)) -> memptr<decltype(t), Res, M MOD, false, true, Ts...>; \
  template<class Res, class M, class ...Ts> \
    requires(std::is_class_v<M>) \
  memptr(Res (* t)(M MOD, Ts..., ...) noexcept) -> memptr<decltype(t), Res, M MOD, true, true, Ts...>;

MEM_PTR_FROM_FREEPTR_NOE_VAR(*)
MEM_PTR_FROM_FREEPTR_NOE_VAR(&)
MEM_PTR_FROM_FREEPTR_NOE_VAR(&&)

#undef MEM_PTR_FROM_FREEPTR_NOE_VAR

  template<class Res, class M>
    requires(!std::is_function_v<Res>)
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

  template<std::size_t N>
  struct static_string {
    consteval static_string(const char(&c)[N]) noexcept {
      for (auto *p = arr.data(); auto ch: c)
        *p++ = ch;
    }

    consteval static_string(std::array<char, N> c) noexcept {
      for (auto *p = arr.data(); auto ch: c)
        *p++ = ch;
    }

    consteval static_string(char prefix, std::array<char, N - 1> c) noexcept {
      auto *p = arr.data();
      *p++ = prefix;
      for (auto ch: c)
        *p++ = ch;
    }

    consteval static_string(std::string_view sv) {
      if (sv.size() != N-1)
        throw;
      for (auto *p = arr.data(); auto ch : sv)
        *p++ = ch;
      arr.back() = '\0';
    }

    constexpr bool operator==(const static_string&) const = default;
    template<std::size_t M>
      requires(N != M)
    constexpr bool operator==(const static_string<M>&) const {
      return false;
    }

    std::array<char, N> arr{};
  };

  template<std::size_t N>
  static_string(char prefix, std::array<char, N> c) -> static_string<N + 1>;

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
            --ix; break;
          case '&':
            if (sv.substr(0, ix).ends_with(" ") ||
                sv.substr(0, ix).ends_with(" &"))
              return ix;
            --ix; break;
          case ')':
            if (sv.substr(0, ix).ends_with("operator ("))
              return ix;
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
            if (sv.substr(0, ix).ends_with("operator ") ||
                sv.substr(0, ix).ends_with("operator <=") ||
                sv.substr(0, ix).ends_with("operator >") ||
                sv.substr(0, ix).ends_with("operator -"))
              return ix;
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
    constexpr std::string_view operator_suffix = "operator ";
#else
    constexpr std::string_view sv = __PRETTY_FUNCTION__;
    constexpr auto last = sv.find_last_not_of(" ])}");
    constexpr std::string_view operator_suffix = "operator";
#endif
      constexpr auto firstx =
          sv.find_last_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789", last);
      constexpr auto firsty = last == firstx ?
          sv.find_last_not_of("+-*/%^&|~!=<>,()[]", last)
          : firstx;
      constexpr auto first =
          last != firstx ? firstx : firsty >= operator_suffix.size() &&
            sv.substr(firsty-operator_suffix.size()+1, operator_suffix.size()) == operator_suffix ?
                firsty : last;
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

  template<static_string S, class ...Args>
  struct accessor_t;

  template<class ... Args>
  struct accessor_t<"", Args...> {
    constexpr static bool always_false = sizeof...(Args) != sizeof...(Args);
    static_assert(always_false, "Cannot able to recognize the type name. Please use the overloaded access");
  };

  template<static_string S>
    requires(S != static_string{""})
  struct accessor_t<S> {
    template<class First, class ...Ts>
      requires(std::is_pointer_v<std::remove_reference_t<First>>)
    constexpr decltype(auto) operator()(First f, Ts &&... ts) const
      noexcept(memptr{get(accessor_t<S, std::remove_reference_t<First>, Ts...>{})}.is_noexcept)
    {
      return std::invoke(get(accessor_t<S, std::remove_reference_t<First>, Ts...>{}),
        std::forward<First>(f), std::forward<Ts>(ts)...);
    }

    template<class First, class ...Ts>
      requires(std::is_class_v<std::remove_reference_t<First>>)
    constexpr decltype(auto) operator()(First &&f, Ts &&... ts) const
      noexcept(memptr{get(accessor_t<S, First &&, Ts...>{})}.is_noexcept)
    {
      return std::invoke(get(accessor_t<S, First &&, Ts...>{}), std::forward<First>(f), std::forward<Ts>(ts)...);
    }
  };

#if not defined(__clang__) and defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wnon-template-friend"
#endif

  template<static_string S, class A, class ...Args>
    requires(S != static_string{""} && std::is_reference_v<A>)
  struct accessor_t<S, A, Args...> : accessor_t<S> {
    friend constexpr auto get(accessor_t);

    template<class ...Ts>
    constexpr decltype(auto) operator()(A f, Ts &&... ts) const
      noexcept(memptr{get(accessor_t{})}.is_noexcept)
    {
      return std::invoke(get(accessor_t{}), std::forward<A>(f), std::forward<Ts>(ts)...);
    }

    friend constexpr decltype(auto) call(accessor_t<S>, A f, Args... args)
    {
        return accessor_t{}(f, std::forward<Args>(args)...);
    }
  };

  template<static_string S, class A, class ...Args>
    requires(S != static_string{""} && std::is_pointer_v<std::remove_reference_t<A>>)
  struct accessor_t<S, A, Args...> :
        accessor_t<S, std::remove_pointer_t<A>&, Args...>,
        accessor_t<S, std::remove_pointer_t<A>&&, Args...> {
    friend constexpr auto get(accessor_t);

    friend constexpr auto get(accessor_t<S, std::remove_pointer_t<A>&, Args...>) {
      if constexpr (std::is_member_pointer_v<decltype(get(accessor_t{}))>)
        return get(accessor_t{});
      else
        return +[] (std::remove_pointer_t<A>& this_, Args&& ... args) {
          return std::invoke(get(accessor_t{}), std::addressof(this_), std::forward<Args>(args)...);
        };
    }

    friend constexpr auto get(accessor_t<S, std::remove_pointer_t<A>&&, Args...>) {
      if constexpr (std::is_member_pointer_v<decltype(get(accessor_t{}))>)
        return get(accessor_t{});
      else
        return +[] (std::remove_pointer_t<A> &&this_, Args&& ... args) {
          return std::invoke(get(accessor_t{}), std::addressof(this_), std::forward<Args>(args)...);
        };
    }

    template<class ...Ts>
    constexpr decltype(auto) operator()(A f, Ts &&... ts) const
      noexcept(memptr{get(accessor_t{})}.is_noexcept)
    {
      return std::invoke(get(accessor_t{}), f, std::forward<Ts>(ts)...);
    }

    friend constexpr decltype(auto) call(accessor_t<S>, A f, Args... args)
    {
      return accessor_t{}(f, std::forward<Args>(args)...);
    }
  };

  template<static_string S, class Base>
    requires(S != static_string{""} && std::is_class_v<Base>)
  struct accessor_t<S, Base> {
    friend constexpr auto get(accessor_t);

    template<class ...Ts>
    constexpr decltype(auto) operator()(Ts &&... ts) const
    noexcept(freeptr{get(accessor_t<S, Base, Ts...>{})}.is_noexcept) {
      if constexpr (std::is_function_v<std::remove_pointer_t<decltype(get(accessor_t<S, Base, Ts...>{}))>>)
        return std::invoke(get(accessor_t<S, Base, Ts...>{}), std::forward<Ts>(ts)...);
      else {
        static_assert(sizeof...(Ts) == 0);
        return *get(accessor_t<S, Base>{});
      }
    }

    friend constexpr decltype(auto) call(accessor_t)
    {
        return accessor_t{}();
    }
  };


  template<static_string S, class Base, class ...Args>
    requires(S != static_string{""} && std::is_class_v<Base> && sizeof...(Args) > 0)
  struct accessor_t<S, Base, Args...> {
    friend constexpr auto get(accessor_t);

    template<class ...Ts>
    constexpr decltype(auto) operator()(Ts &&... ts) const
      noexcept(freeptr{get(accessor_t{})}.is_noexcept) {
      return std::invoke(get(accessor_t{}), std::forward<Ts>(ts)...);
    }

    friend constexpr decltype(auto) call(accessor_t, Args ...args)
    {
      return accessor_t{}(std::forward<Args>(args)...);
    }
  };

#if not defined(__clang__) and defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif


  template<auto T, class = void, class Type = decltype(T),
            auto str = name_impl<decltype(T.ptr), T.ptr>()
            >
  struct access_impl;

  template<auto T, class Ptr, class Res, class Class, bool noexc, bool varg, class ...Args,
            auto str> requires(std::is_reference_v<Class>)
  struct access_impl<T, void, memptr<Ptr, Res, Class, noexc, varg, Args...>, str> :
      accessor_t<str, Class, Args...> {
    friend constexpr auto get(accessor_t<str, Class, Args...>) { return memptr{+T}.ptr; }
  };

  template<auto T, class Ptr, class Res, class Class, bool noexc, bool varg, class ...Args,
            auto str> requires(std::is_pointer_v<Class>)
  struct access_impl<T, void, memptr<Ptr, Res, Class, noexc, varg, Args...>, str> :
      accessor_t<str, Class, Args...> {
    using rptr = std::remove_pointer_t<Class>;
    friend constexpr auto get(accessor_t<str, Class, Args...>) { return memptr{+T}.ptr; }
  };


  template<auto T, class Ptr, class Res, class Class,
            auto str> requires(std::is_class_v<Class>)
  struct access_impl<T, void, memptr<Ptr, Res, Class, true, false>, str> :
      accessor_t<str, Class *>,
      accessor_t<str, const Class *>,
      accessor_t<str, volatile Class *>,
      accessor_t<str, const volatile Class *> {
    friend constexpr auto get(accessor_t<str, Class *>) { return memptr{+T}.ptr; }
    friend constexpr auto get(accessor_t<str, const Class *>) { return memptr{+T}.ptr; }
    friend constexpr auto get(accessor_t<str, volatile Class *>) { return memptr{+T}.ptr; }
    friend constexpr auto get(accessor_t<str, const volatile Class *>) { return memptr{+T}.ptr; }
  };

  template<auto T, class Base, class Ptr, class Res, bool noexc, bool varg, class ...Args,
            auto str>
    requires(!std::is_void_v<Base>)
  struct access_impl<T, Base, freeptr<Ptr, Res, noexc, varg, Args...>, str> :
      accessor_t<str, Base, Args...> {
    friend constexpr auto get(accessor_t<str, Base, Args...>) { return freeptr{+T}.ptr; }
  };

  template<auto T, class = void, static_string = "">
  struct access;

  template<static_string T>
  concept is_operator = std::string_view{T.data.data(), T.data.size()}.find_first_not_of("+-*/%^&|~!=<>,()[]") == std::string_view::npos;

  template<auto T, class Res = typename decltype(memptr{T})::res_type>
  concept is_conversion_op =
    decltype(memptr{T})::args == 0 && !decltype(memptr{T})::has_vararg &&
    !std::is_void_v<Res> &&
#if not defined(__clang__) && defined(__GNUC__)
    static_string{name_impl<decltype(T), T>()} == static_string{"__conv_op"}
#else
    !is_operator<name_impl<decltype(T), T>()> &&
    static_string{name_impl<decltype(T), T>()} ==
      static_string{type_name<Res>()}
#endif
  ;

  // member / function
  template<auto T> requires(requires { memptr{T}; } &&
    !is_conversion_op<T>)
  struct access<T> : access_impl<memptr{T}> {
  };

  // conversion operator
  template<auto T> requires(requires { memptr{T}; } &&
    is_conversion_op<T>)
  struct access<T> : access_impl<memptr{T}

#if not defined(__clang__) && defined(__GNUC__)
    , void, decltype(memptr{T}), type_name<typename decltype(memptr{T})::res_type>()
#endif
  > {};

  // conversion operator with unique name
  template<auto T, static_string str> requires(requires { memptr{T}; } &&
    is_conversion_op<T>)
  struct access<T, void, str> : access_impl<memptr{T}, void, decltype(memptr{T}), str> {};

  // static member/function
  template<auto T, class V> requires(std::same_as<decltype(T), decltype(+T)> && requires { freeptr{T}; } &&
             !std::is_void_v<V>)
  struct access<T, V> : access_impl<freeptr{T}, V> {
  };

  // constructor
  template<auto T, class V> requires(!std::same_as<decltype(T), decltype(+T)> &&
             !std::is_void_v<V> &&
             std::same_as<V, typename decltype(freeptr{+T})::res_type>)
  struct access<T, V> : access_impl<T, V, decltype(freeptr{+T}),
                                    static_string{"construct"}
                                    > {
  };

  // destruct_at
  template<auto T, class V> requires(!std::same_as<decltype(T), decltype(+T)> && !std::is_void_v<V> &&
             std::is_void_v<typename decltype(freeptr{+T})::res_type>)
  struct access<T, V> : access_impl<T, V, decltype(freeptr{+T}),
                                    static_string{'~', type_name<V>()}
                                    > {
  };

  // destruct_at with unique name
  template<auto T, class V, static_string str> requires(!std::same_as<decltype(T), decltype(+T)> && !std::is_void_v<V> &&
             std::is_void_v<typename decltype(freeptr{+T})::res_type>)
  struct access<T, V, str> : access_impl<T, V, decltype(freeptr{+T}),
                                    static_string{'~', str.arr}
  > {};

  // construct_at
  template<auto T, class V> requires(!std::same_as<decltype(T), decltype(+T)> && !std::is_void_v<V> &&
             std::same_as<V &, typename decltype(freeptr{+T})::res_type>)
  struct access<T, V> : access_impl<T, V, decltype(freeptr{+T}),
                                    type_name<V>()
                                    > {};

  // construct_at with unique name
  template<auto T, class V, static_string str> requires(!std::same_as<decltype(T), decltype(+T)> && !std::is_void_v<V> &&
             std::same_as<V &, typename decltype(freeptr{+T})::res_type>)
  struct access<T, V, str> : access_impl<T, V, decltype(freeptr{+T}), str
                                    > {};

  // base class conversion
  template<auto T> requires(!std::same_as<decltype(T), decltype(+T)> &&
             std::is_pointer_v<typename decltype(freeptr{+T})::res_type> && requires { memptr{+T}; })
  struct access<T, void> : access_impl<T, void, decltype(memptr{+T}),
                                   type_name<std::remove_pointer_t<typename decltype(freeptr{+T})::res_type>>()
                                    > {
  };

  // base class conversion with unique name
  template<auto T, static_string str> requires(!std::same_as<decltype(T), decltype(+T)> &&
             std::is_pointer_v<typename decltype(freeptr{+T})::res_type> && requires { memptr{+T}; })
  struct access<T, void, str> : access_impl<T, void, decltype(memptr{+T}), str> {};

  // default or specific arguments
  template<auto T, static_string S, class V = void>
  struct unique_access;
  template<auto T, static_string S>
    requires(std::is_member_object_pointer_v<decltype(T)>)
  struct unique_access<T, S, void> : access_impl<memptr{T}, void,
                                                 decltype(memptr{T}), S
                                                 > {
  };

  template<auto T, static_string S>
    requires(!std::same_as<decltype(T), decltype(+T)> && requires { memptr{+T}; })
  struct unique_access<T, S, void> : access_impl<T, void,
                                                 decltype(memptr{+T}), S
                                                 > {
  };

  template<auto T, static_string S, class V>
    requires(!std::same_as<decltype(T), decltype(+T)> && !std::is_void_v<V>)
  struct unique_access<T, S, V> : access_impl<T, V, decltype(freeptr{+T}), S
                                              > {
  };


  template<static_string S, class ...Args>
  constexpr static accessor_t<S, Args...> accessor{};

  template<class T, class Base, static_string rename = "">
  struct type_access
      : accessor_t<rename, Base> {
    friend constexpr auto get(accessor_t<rename, Base>) {
      return static_cast<T *>(nullptr);
    }
  };

  template<class T, class Base>
  struct type_access<T, Base>
      : accessor_t<type_name<T>(), Base> {
    friend constexpr auto get(accessor_t<type_name<T>(), Base>) {
      return static_cast<T *>(nullptr);
    }
  };

  template<static_string S, class Base>
  using type_accessor = std::remove_pointer_t<decltype(accessor_t<S, Base>{}())>;


#if defined(__clang__)
#define ACCESS_PRIVATE_GET_ARGE(...)
#define ACCESS_PRIVATE_GET_ARG0(M, x) M(x, t0)
#define ACCESS_PRIVATE_GET_ARG1(M, x, ...) M(x, t1), ACCESS_PRIVATE_GET_ARG0(M, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARG2(M, x, ...) M(x, t2), ACCESS_PRIVATE_GET_ARG1(M, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARG3(M, x, ...) M(x, t3), ACCESS_PRIVATE_GET_ARG2(M, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARG4(M, x, ...) M(x, t4), ACCESS_PRIVATE_GET_ARG3(M, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARG5(M, x, ...) M(x, t5), ACCESS_PRIVATE_GET_ARG4(M, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARG6(M, x, ...) M(x, t6), ACCESS_PRIVATE_GET_ARG5(M, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARG7(M, x, ...) M(x, t7), ACCESS_PRIVATE_GET_ARG6(M, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARG8(M, x, ...) M(x, t8), ACCESS_PRIVATE_GET_ARG7(M, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARG9(M, x, ...) M(x, t9), ACCESS_PRIVATE_GET_ARG8(M, __VA_ARGS__)

#define ACCESS_PRIVATE_GET_ARG(i, ...) ACCESS_PRIVATE_GET_ARGX(i, __VA_ARGS__)
#define ACCESS_PRIVATE_GET_ARGX(i, ...) ACCESS_PRIVATE_GET_ARG##i(__VA_ARGS__)

#define ACCESS_PRIVATE_VA_NUM_ARGS(...) ACCESS_PRIVATE_VA_NUM_ARGS_IMPL(__VA_ARGS__ __VA_OPT__(,) 9,8,7,6,5,4,3,2,1,0,E)
#define ACCESS_PRIVATE_VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N

#define ACCESS_PRIVATE_FOREACH_ARG(M, ...) ACCESS_PRIVATE_GET_ARG(ACCESS_PRIVATE_VA_NUM_ARGS(__VA_ARGS__), M, __VA_ARGS__)
#define ACCESS_PRIVATE_DECLARE(type, name) type name
#define ACCESS_PRIVATE_FORWARD(type, name) std::forward<type>(name)

#define constructor(T, ...) [] (ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_DECLARE, __VA_ARGS__)) \
  noexcept(std::is_nothrow_constructible_v<T __VA_OPT__(,) __VA_ARGS__>) {                                          \
    return T{ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_FORWARD, __VA_ARGS__)}; \
}

#define constructing_at(T, ...) [] (T* p __VA_OPT__(,) ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_DECLARE, __VA_ARGS__)) \
  noexcept(std::is_nothrow_constructible_v<T __VA_OPT__(,) __VA_ARGS__>) -> T& {                                         \
    return *new (p) T{ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_FORWARD, __VA_ARGS__)};    \
}
#define destructing_at(T) [] (T& p) noexcept(std::is_nothrow_destructible_v<T>) { p.~T(); }

#define call_member_function_with(T, member, ...) \
template struct unique_access<[] (T* p __VA_OPT__(,) ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_DECLARE, __VA_ARGS__))\
  noexcept(noexcept(p->member(ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_FORWARD, __VA_ARGS__))))\
  -> decltype(auto) {         \
    return p->member(ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_FORWARD, __VA_ARGS__));  \
}, #member>

#define call_static_function_with(T, member, ...) \
template struct unique_access<[] (ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_DECLARE, __VA_ARGS__)) \
  noexcept(noexcept(T::member(ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_FORWARD, __VA_ARGS__))))\
  -> decltype(auto) {         \
    return T::member(ACCESS_PRIVATE_FOREACH_ARG(ACCESS_PRIVATE_FORWARD, __VA_ARGS__));  \
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
