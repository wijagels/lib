// Copyright 2017 William Jagels
#ifndef INCLUDE_ALLOCATOR_HPP_
#define INCLUDE_ALLOCATOR_HPP_
#include <memory>

namespace wijagels {
template <typename T>
struct pointer_traits {
  using reference = T&;
  using const_reference = const T&;
};

template <>
struct pointer_traits<void> {};

template <typename T = void>
struct BasicAllocator : public pointer_traits<T> {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using pointer = T*;
  using const_pointer = const T*;
  using difference_type =
      typename std::pointer_traits<pointer>::difference_type;

  BasicAllocator() = default;

  ~BasicAllocator() = default;

  template <typename U>
  BasicAllocator(const BasicAllocator<U>&) noexcept {}

  T* allocate(size_t, const void* = nullptr) {
    auto ptr = new T;
    return ptr;
  }

  void deallocate(T* ptr, size_t) { delete ptr; }

  template <typename U>
  struct rebind {
    using other = BasicAllocator<U>;
  };
};

template <typename T, typename U>
constexpr bool operator==(const BasicAllocator<T>&,
                          const BasicAllocator<U>&) noexcept;

template <typename T, typename U>
constexpr bool operator!=(const BasicAllocator<T>&,
                          const BasicAllocator<U>&) noexcept;
}  // namespace wijagels
#endif  // INCLUDE_ALLOCATOR_HPP_
