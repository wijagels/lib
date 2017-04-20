// Copyright 2017 William Jagels
#ifndef INCLUDE_LIST_HPP_
#define INCLUDE_LIST_HPP_
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>

namespace wijagels {
namespace detail {
struct Node_Base_ {
  Node_Base_() : Node_Base_{this, this} {}
  Node_Base_(Node_Base_ *prev, Node_Base_ *next) : prev_{prev}, next_{next} {}
  Node_Base_(Node_Base_ &&other)
      : prev_{std::move(other.prev_)}, next_{std::move(other.next_)} {
    other.next_ = &other;
    other.prev_ = &other;
  }

  Node_Base_ &operator=(const Node_Base_ &) = default;

  Node_Base_ *prev_, *next_;
};

template <typename T>
struct Node_ : Node_Base_ {
  Node_(Node_ *prev, Node_ *next, const T &data)
      : Node_Base_{prev, next}, data_{data} {}

  explicit Node_(const T &data) : Node_{this, this, data} {}

  Node_(Node_ *prev, Node_ *next, T &&data)
      : Node_Base_{prev, next}, data_{data} {}

  template <typename... Args>
  explicit Node_(Args &&... args)
      : Node_{this, this, std::forward<Args>(args)...} {}

  template <typename... Args>
  explicit Node_(Node_ *prev, Node_ *next, Args &&... args)
      : Node_Base_{prev, next}, data_{args...} {}

  Node_(Node_ &&other) : Node_Base_{other}, data_{std::move(other.data_)} {}

  Node_ &operator=(const Node_ &) = default;

  T &operator*() { return data_; }

  T *operator->() { return &data_; }

  friend void swap(Node_ &lhs, Node_ &rhs) noexcept {
    std::swap(lhs.next_, rhs.next_);
    std::swap(lhs.prev_, rhs.prev_);
  }

  T data_;
};
}  // namespace detail

template <typename T, class Allocator = std::allocator<T>>
class list {
  using Node_Base = detail::Node_Base_;
  using Node = detail::Node_<T>;

 public:
  class iterator;
  class const_iterator;
  using value_type = T;
  using allocator_type = Allocator;
  using node_allocator_type = typename std::allocator_traits<
      allocator_type>::template rebind_alloc<Node>;
  using node_alloc_traits = typename std::allocator_traits<node_allocator_type>;
  using size_type = size_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 public:
  class iterator {
   public:
    Node_Base *node_;
    friend list;

   public:
    using value_type = T;
    using difference_type = size_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer =
        typename std::allocator_traits<Allocator>::const_pointer;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit iterator(Node_Base *node) : node_{node} {}

    iterator &operator++() {
      node_ = node_->next_;
      return *this;
    }

    iterator operator++(int) {
      iterator ret = iterator{*this};
      node_ = node_->next_;
      return ret;
    }

    iterator &operator--() {
      node_ = node_->prev_;
      return *this;
    }

    iterator operator--(int) {
      iterator ret = iterator{*this};
      node_ = node_->prev_;
      return ret;
    }

    reference operator*() const { return static_cast<Node *>(node_)->data_; }

    pointer operator->() const { return &(static_cast<Node *>(node_)->data_); }

    friend bool operator==(const iterator &lhs, const iterator &rhs) {
      return lhs.node_ == rhs.node_;
    }

    friend bool operator!=(const iterator &lhs, const iterator &rhs) {
      return lhs.node_ != rhs.node_;
    }

    friend difference_type distance(const iterator &lhs, const iterator &rhs) {
      size_type d = 0;
      auto it = lhs;
      while (it++ != rhs) d++;
      return d;
    }
  };

  class const_iterator {
    const Node_Base *node_;
    friend list;

   public:
    using value_type = T;
    using difference_type = size_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer =
        typename std::allocator_traits<Allocator>::const_pointer;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit const_iterator(const Node_Base *node) : node_{node} {}

    const_iterator(const iterator &other) : node_{other.node_} {}

    const_iterator &operator++() {
      node_ = node_->next_;
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator ret = const_iterator{*this};
      node_ = node_->next_;
      return ret;
    }

    const_iterator &operator--() {
      node_ = node_->prev_;
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator ret = const_iterator{*this};
      node_ = node_->prev_;
      return ret;
    }

    const_reference operator*() const {
      return static_cast<const Node *>(node_)->data_;
    }

    const_pointer operator->() const {
      return &static_cast<const Node *>(node_)->data_;
    }

    friend bool operator==(const const_iterator &lhs,
                           const const_iterator &rhs) {
      return lhs.node_ == rhs.node_;
    }

    friend bool operator!=(const const_iterator &lhs,
                           const const_iterator &rhs) {
      return lhs.node_ != rhs.node_;
    }

    friend difference_type distance(const const_iterator &lhs,
                                    const const_iterator &rhs) {
      difference_type d = 0;
      auto it = lhs;
      while (it++ != rhs) d++;
      return d;
    }

   private:
    iterator un_const() { return iterator{const_cast<Node_Base *>(node_)}; }
  };

 public:
  list() : list{Allocator()} {}

  explicit list(const Allocator &alloc) : head_{}, alloc_{alloc} {}

  list(size_type count, const T &value, const Allocator &alloc = Allocator())
      : head_{}, alloc_{alloc} {
    for (; count > 0; --count) push_back(value);
  }

  explicit list(size_type count, const Allocator &alloc = Allocator())
      : list{alloc} {
    for (; count > 0; --count) emplace_back();
  }

  template <class InputIt>
  list(InputIt first, InputIt last, const Allocator &alloc = Allocator())
      : list{alloc} {
    for (; first != last; ++first) {
      push_back(*first);
    }
  }

  list(const list &other) : head_{}, alloc_{} {
    alloc_ = std::allocator_traits<
        allocator_type>::select_on_container_copy_construction(other.alloc_);
    for (const_reference e : other) push_back(e);
  }

  list(const list &other, const Allocator &alloc) : list{alloc} {
    for (const_reference e : other) push_back(e);
  }

  list(size_type count, const_reference value) : list{} {
    while (count--) {
      push_back(value);
    }
  }

  list(list &&other)
      : alloc_{std::move(other.alloc_)}, node_alloc_{std::move(other.alloc_)} {
    splice(cend(), other);
  }

  list(std::initializer_list<T> init, const Allocator &alloc = Allocator())
      : head_{}, alloc_{alloc} {
    for (auto &e : init) {
      emplace_back(e);
    }
  }

  ~list() { clear(); }

  list &operator=(const list &other) {
    clear();
    if (std::allocator_traits<
            allocator_type>::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
    }
    for (auto e : other) {
      push_back(e);
    }
    return *this;
  }

  list &operator=(list &&other) {
    clear();
    if (std::allocator_traits<
            allocator_type>::propagate_on_container_move_assignment::value) {
      alloc_ = std::move(other.alloc_);
      splice(cend(), other);
    } else {
      for (auto &e : other) {
        emplace_back(std::move(e));
      }
      other.clear();
    }
    return *this;
  }

  list &operator=(std::initializer_list<T> ilist) {
    for (auto e : ilist) {
      push_back(e);
    }
    return *this;
  }

  void assign(size_type count, const T &value) {
    clear();
    for (; count > 0; --count) {
      push_back(value);
    }
  }

  template <class InputIt>
  void assign(InputIt first, InputIt last) {
    clear();
    for (; first != last; ++first) {
      push_back(*first);
    }
  }

  void assign(std::initializer_list<T> ilist) {
    clear();
    for (const auto &e : ilist) {
      push_back(e);
    }
  }

  allocator_type get_allocator() { return alloc_; }

  /* Element access */

  reference front() { return *(begin()); }
  const_reference front() const { return *(begin()); }

  reference back() { return *(--end()); }
  const_reference back() const { return *(--end()); }

  /* Iterators */

  iterator begin() noexcept { return iterator{head_.next_}; }
  iterator end() noexcept { return iterator{&head_}; }

  const_iterator begin() const noexcept { return const_iterator{head_.next_}; }
  const_iterator end() const noexcept { return const_iterator{&head_}; }

  const_iterator cbegin() const noexcept { return const_iterator{head_.next_}; }
  const_iterator cend() const noexcept { return const_iterator{&head_}; }

  reverse_iterator rbegin() noexcept {
    return reverse_iterator{iterator{&head_}};
  }
  reverse_iterator rend() noexcept {
    return reverse_iterator{iterator{head_.next_}};
  }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator{&head_};
  }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator{head_.next_};
  }

  /* Capacity */

  bool empty() const noexcept { return cbegin() == cend(); }

  size_type size() const noexcept { return std::distance(begin(), end()); }

  constexpr size_type max_size() const noexcept {
    return std::numeric_limits<size_type>::max();
  }

  /* Modifiers */

  void clear() noexcept {
    while (begin() != end()) erase(begin());
  }

 private:
  inline void link_(Node_Base *first, Node_Base *last) {
    first->next_ = last;
    last->prev_ = first;
  }

  template <typename... Args>
  inline void link_(Node_Base *first, Node_Base *second, Args... rest) {
    link_(first, second);
    link_(second, rest...);
  }

 public:
  iterator insert(const iterator &pos, const_reference data) {
    using std::allocator_traits;
    Node *node = node_alloc_traits::allocate(node_alloc_, 1);
    node_alloc_traits::construct(node_alloc_, node, data);
    link_(pos.node_->prev_, node, pos.node_);
    return iterator{node};
  }

  template <class... Args>
  iterator emplace(const iterator &pos, Args &&... args) {
    Node *node = node_alloc_traits::allocate(node_alloc_, 1);
    node_alloc_traits::construct(node_alloc_, node,
                                 std::forward<Args>(args)...);
    link_(pos.node_->prev_, node, pos.node_);
    return iterator{node};
  }

  void erase(const iterator &iter) {
    link_(iter.node_->prev_, iter.node_->next_);
    node_alloc_traits::destroy(node_alloc_, iter.node_);
    node_alloc_traits::deallocate(node_alloc_, static_cast<Node *>(iter.node_),
                                  1);
  }

  void push_back(const_reference data) { insert(end(), data); }

  template <class... Args>
  reference emplace_back(Args &&... args) {
    return *emplace(end(), std::forward<Args>(args)...);
  }

  void pop_back() { erase(--end()); }

  void push_front(const_reference data) { insert(begin(), data); }

  template <class... Args>
  reference emplace_front(Args &&... args) {
    return *emplace(begin(), std::forward<Args>(args)...);
  }

  void pop_front() { erase(begin()); }

  void resize(size_type count) { resize(count, value_type{}); }

  void resize(size_type count, const value_type &value) {
    size_type sz = size();
    if (sz == count) {
      return;
    } else if (sz < count) {
      while (sz++ < count) {
        push_back(value);
      }
    } else {  // sz > count
      sz -= count;
      while (sz--) pop_back();
    }
  }

  void swap(list &other) noexcept {
    if (std::allocator_traits<
            allocator_type>::propagate_on_container_swap::value) {
      std::swap(alloc_, other.alloc_);
    }
    if (node_alloc_traits::propagate_on_container_swap::value) {
      std::swap(node_alloc_, other.node_alloc_);
    }
    std::swap(head_, other.head_);
  }

  /* Operations */

  void merge(list &other) { merge(std::move(other)); }

  void merge(list &&other) {
    merge(other,
          [](const_reference lhs, const_reference rhs) { return lhs < rhs; });
  }

  template <class Compare>
  void merge(list &other, Compare comp) {
    merge(std::move(other), comp);
  }

  template <class Compare>
  void merge(list &&other, Compare comp) {
    // make sure we aren't merging with self
    if (this == &other) return;

    auto lhs = begin();
    auto rhs = other.begin();

    for (; rhs != other.end() && lhs != end(); ++lhs) {
      if (comp(*rhs, *lhs)) {
        // node.prev <-> node.next
        // lhs.prev <-> rhs <-> lhs <-> lhs.next
        auto node = rhs.node_;
        ++rhs;
        link_(node->prev_, node->next_);
        link_(lhs.node_->prev_, node, lhs.node_);
        lhs.node_ = node;
      }
    }
    // Appends remainder if we didn't reach the end
    if (rhs != other.end()) splice(cend(), other);
  }

  void splice(const_iterator pos, list &other) {
    splice(pos, std::move(other));
  }

  void splice(const_iterator pos, list &&other) {
    splice(pos, other, other.cbegin(), other.cend());
  }

  void splice(const_iterator pos, list &other, const_iterator it) {
    splice(pos, std::move(other), it);
  }

  void splice(const_iterator pos, list &&other, const_iterator it) {
    splice(pos, other, it, other.cend());
  }

  void splice(const_iterator pos, list &other, const_iterator first,
              const_iterator last) {
    splice(pos, std::move(other), first, last);
  }

  void splice(const_iterator pos, list &&, const_iterator first,
              const_iterator last) {
    iterator ipos = pos.un_const();
    iterator ifirst = first.un_const();
    iterator ilast = last.un_const();

    auto final_node = ilast.node_->prev_;
    // Fix pointers in other
    link_(ifirst.node_->prev_, ilast.node_);

    link_(ipos.node_->prev_, ifirst.node_);
    link_(final_node, ipos.node_);
  }

  void remove(const T &value) {
    remove_if([&value](const_reference operand) { return operand == value; });
  }

  template <class UnaryPredicate>
  void remove_if(UnaryPredicate p) {
    for (iterator it = begin(); it != end(); ++it)
      if (p(*it)) erase(it--);
    // --begin() results in end(), ++end() results in begin()
  }

  void reverse() noexcept {
    iterator it = end();
    do {
      std::swap(it.node_->prev_, it.node_->next_);
    } while (++it != end());
  }

  void unique() {
    unique([](const_reference lhs, const_reference rhs) { return lhs == rhs; });
  }

  template <class BinaryPredicate>
  void unique(BinaryPredicate p) {
    if (empty()) return;
    auto current = *begin();
    for (iterator it = ++begin(); it != end(); ++it) {
      if (p(current, *it))
        erase(it--);
      else
        current = *it;
    }
  }

  void sort() {
    sort([](const_reference lhs, const_reference rhs) { return lhs < rhs; });
  }

  template <class Compare>
  void sort(Compare comp) {
    if (empty() || ++begin() == end()) return;
    auto tortise = cbegin();
    auto hare = cbegin();
    while (hare != cend() && ++hare != cend()) {
      ++tortise;
      ++hare;
    }
    list left, right;
    left.splice(left.cend(), *this, cbegin(), tortise);
    right.splice(right.cend(), *this);
    left.sort(comp);
    right.sort(comp);
    left.merge(right, comp);
    splice(cend(), left);
  }

  /* Friend functions */

  /*
   * Allows for argument-dependent lookup with std::swap
   */
  friend void swap(list &lhs, list &rhs) noexcept { lhs.swap(rhs); }

  friend bool operator==(const list &lhs, const list &rhs) {
    auto lit = lhs.begin();
    auto rit = rhs.begin();
    for (; lit != lhs.end() && rit != rhs.end(); ++lit, ++rit) {
      if (!(*lit == *rit)) return false;
    }
    return lit == lhs.end() && rit == rhs.end();
  }

  friend bool operator!=(const list &lhs, const list &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const list &lhs, const list &rhs) {
    auto lit = lhs.begin();
    auto rit = rhs.begin();
    for (; lit != lhs.end() && rit != rhs.end(); ++lit, ++rit) {
      if (*lit < *rit) return true;
      if (*rit < *lit) return false;
    }
    return lit == lhs.end() && rit != rhs.end();
  }

  friend bool operator<=(const list &lhs, const list &rhs) {
    auto lit = lhs.begin();
    auto rit = rhs.begin();
    for (; lit != lhs.end() && rit != rhs.end(); ++lit, ++rit) {
      if (*lit < *rit) return true;
      if (*rit < *lit) return false;
    }
    return lit == lhs.end();
  }

  friend bool operator>(const list &lhs, const list &rhs) { return rhs < lhs; }

  friend bool operator>=(const list &lhs, const list &rhs) {
    return rhs <= lhs;
  }

#ifdef LIST_DBG_ON
  bool verify_integrity() {
    for (iterator it = begin(); it != end(); it++) {
      if (it.node_ != it.node_->prev_->next_ ||
          it.node_ != it.node_->next_->prev_)
        return false;
    }
    return true;
  }
#endif

#ifdef LIST_DBG_ON
 public:
#else
 private:
#endif
  Node_Base head_;
  allocator_type alloc_;
  node_allocator_type node_alloc_;
};

/*
#if __cplusplus > 201402L
namespace pmr {
template <class T>
using list = list<T, std::pmr::polymorphic_allocator<T>>;
}
#endif  // __cplusplus > 201402L
*/
}  // namespace wijagels

#endif  // INCLUDE_LIST_HPP_
