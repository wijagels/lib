// Copyright 2017 William Jagels
#pragma once
#include "List.hpp"
#include <utility>

namespace wijagels {
template <typename T, typename Container = list<T>>
class stack {
 public:
  using container_type = Container;
  using value_type = typename container_type::value_type;
  using size_type = typename container_type::size_type;
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;

  explicit stack(const container_type &cont) : c{cont} {}
  explicit stack(container_type &&cont = container_type()) : c{cont} {}
  stack(const stack &other) = default;
  stack(stack &&other) = default;
  template <class Alloc>
  explicit stack(const Alloc &alloc) : c{alloc} {}
  template <class Alloc>
  stack(const container_type &cont, const Alloc &alloc) : c{cont, alloc} {}
  template <class Alloc>
  stack(container_type &&cont, const Alloc &alloc) : c{cont, alloc} {}
  template <class Alloc>
  stack(const stack &other, const Alloc &alloc) : c{other.c, alloc} {}
  template <class Alloc>
  stack(stack &&other, const Alloc &alloc) : c{std::move(other.c), alloc} {}

  stack &operator=(const stack &other) = default;
  stack &operator=(stack &&other) = default;

  /* Element access */
  reference top() { return c.back(); }

  const_reference top() const { return c.back(); }

  bool empty() const { return c.empty(); }

  /* Modifiers */
  void push(const value_type &value) { c.push_back(value); }
  void push(value_type &&value) { c.push_back(value); }

  template <class... Args>
  reference emplace(Args &&... args) {
    return c.emplace_back(std::forward<Args>(args)...);
  }

  void pop() { c.pop_back(); }

  void swap(stack &other) { c.swap(other.c); }

 private:
  container_type c;
};
template <class T, class Container>
bool operator==(const stack<T, Container> &lhs,
                const stack<T, Container> &rhs) {
  return lhs.c == rhs.c;
}
template <class T, class Container>
bool operator!=(const stack<T, Container> &lhs,
                const stack<T, Container> &rhs) {
  return lhs.c != rhs.c;
}
template <class T, class Container>
bool operator<(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
  return lhs < rhs;
}
template <class T, class Container>
bool operator<=(const stack<T, Container> &lhs,
                const stack<T, Container> &rhs) {
  return lhs <= rhs;
}
template <class T, class Container>
bool operator>(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
  return lhs > rhs;
}
template <class T, class Container>
bool operator>=(const stack<T, Container> &lhs,
                const stack<T, Container> &rhs) {
  return lhs >= rhs;
}
}  // namespace wijagels
