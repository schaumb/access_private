#include <utility>
#include <type_traits>

namespace detail {

template <typename Tag, typename Tag::ptr_type PtrValue> struct private_cast {
  friend typename Tag::ptr_type get(Tag) { return PtrValue; }
};

template <typename Tag, typename PtrType> struct private_cast_helper {
  using ptr_type = PtrType;
  friend ptr_type get(Tag);
};

} // namespace detail

// @param PtrTypeKind E.g if we have "class A", then it can be "A::*" in case of
// members, or it can be "*" in case of statics.
#define _ACCESS_PRIVATE(Class, Type, Name, PtrTypeKind)                        \
  namespace detail {                                                           \
  using Class##_##Name##_alias = Type;                                         \
  struct Class##_##Name                                                        \
      : private_cast_helper<Class##_##Name,                                    \
                            Class##_##Name##_alias PtrTypeKind> {};            \
  template struct private_cast<Class##_##Name, &Class::Name>;                  \
  }

#define ACCESS_PRIVATE_FIELD(Class, Type, Name)                                \
  _ACCESS_PRIVATE(Class, Type, Name, Class::*)                                 \
  namespace access_private {                                                   \
  Type &Name(Class &&t) { return t.*get(detail::Class##_##Name{}); }           \
  Type &Name(Class &t) { return t.*get(detail::Class##_##Name{}); }            \
  const Type &Name(const Class &t) {                                           \
    return t.*get(detail::Class##_##Name{});                                   \
  }                                                                            \
  }

#define ACCESS_PRIVATE_FUN(Class, Type, Name)                                  \
  _ACCESS_PRIVATE(Class, Type, Name, Class::*)                                 \
  namespace call_private {                                                     \
  template <typename Obj,                                                      \
            std::enable_if_t<std::is_same<std::remove_reference_t<Obj>,        \
                                          Class>::value> * = nullptr,          \
            typename... Args>                                                  \
  auto Name(Obj &&o, Args &&... args) {                                        \
    return (std::forward<Obj>(o).*                                             \
            get(detail::Class##_##Name{}))(std::forward<Args>(args)...);       \
  }                                                                            \
  }

#define ACCESS_PRIVATE_STATIC_FIELD(Class, Type, Name)                         \
  _ACCESS_PRIVATE(Class, Type, Name, *)                                        \
  namespace access_private_static {                                            \
  namespace Class {                                                            \
  Type &Name() { return *get(detail::Class##_##Name{}); }                      \
  }                                                                            \
  }

#define ACCESS_PRIVATE_STATIC_FUN(Class, Type, Name)                           \
  _ACCESS_PRIVATE(Class, Type, Name, *)                                        \
  namespace call_private_static {                                              \
  namespace Class {                                                            \
  template <typename... Args> auto Name(Args &&... args) {                     \
    return get(detail::Class##_##Name{})(std::forward<Args>(args)...);         \
  }                                                                            \
  }                                                                            \
  }
