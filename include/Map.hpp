// Copyright 2017 William Jagels
#ifndef INCLUDE_MAP_HPP_
#define INCLUDE_MAP_HPP_
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include "SkipList.hpp"

namespace wijagels {
namespace detail {
template <class Iterator, class NodeType>
struct InsertReturnType {
  Iterator position;
  bool inserted;
  NodeType node;
};
}  // namespace detail

template <class Key, class T, class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
class map {
 public:
  /* Aliases */
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Compare;
  using allocator_type = Allocator;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  /* Comparison functor for value_type */
  class value_compare {
   protected:
    friend map;
    explicit value_compare(Compare c) : c_{c} {}
    const Compare c_;

   public:
    constexpr bool operator()(const value_type& lhs,
                              const value_type& rhs) const {
      return c_(lhs.first, rhs.first);
    }
    constexpr bool operator()(const key_type& lhs,
                              const value_type& rhs) const {
      return c_(lhs, rhs.first);
    }
    constexpr bool operator()(const value_type& lhs,
                              const key_type& rhs) const {
      return c_(lhs.first, rhs);
    }
    constexpr bool operator()(const key_type& lhs, const key_type& rhs) const {
      return c_(lhs, rhs);
    }
  };
  using container_type = skiplist<value_type, value_compare, allocator_type>;
  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;
  using reverse_iterator = typename container_type::reverse_iterator;
  using const_reverse_iterator =
      typename container_type::const_reverse_iterator;
  using node_type = typename container_type::node_type;
  using insert_return_type = detail::InsertReturnType<iterator, node_type>;

  /* Member functions */
  /* Constructors */
  map() : map{Compare()} {}

  explicit map(const Compare& comp, const Allocator& alloc = Allocator{})
      : comp_{comp},
        val_comp_{comp},
        alloc_{alloc},
        container_{val_comp_, alloc_} {}

  explicit map(const Allocator& alloc) : map{Compare{}, alloc} {}

  template <class InputIterator>
  map(InputIterator first, InputIterator last, const Compare& comp = Compare(),
      const Allocator& alloc = Allocator())
      : map{comp, alloc} {
    insert(first, last);
  }

  template <class InputIterator>
  map(InputIterator first, InputIterator last, const Allocator& alloc)
      : map{first, last, Compare(), alloc} {}

  map(const map& other) = default;

  map(const map& other, const Allocator& alloc);

  map(map&& other);

  map(map&& other, const Allocator& alloc);

  map(std::initializer_list<value_type> init, const Compare& comp = Compare(),
      const Allocator& alloc = Allocator())
      : map{comp, alloc} {
    insert(init);
  }

  map(std::initializer_list<value_type> init, const Allocator& alloc)
      : map{alloc} {
    insert(init);
  }

  /* Destructor */
  ~map() = default;

  /* Assignment */
  map& operator=(const map& other) {
    val_comp_ = other.val_comp_;
    alloc_ = other.alloc_;
    container_ = other.container_;
  }

  map& operator=(map&& other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value&&
          std::is_nothrow_move_assignable<Compare>::value) {
    val_comp_ = other.val_comp_;
    if (std::allocator_traits<
            allocator_type>::propagate_on_container_move_assignment()) {
      alloc_ = other.alloc_;
    }
    container_ = std::move(other.container_);
  }

  map& operator=(std::initializer_list<value_type> ilist) {
    container_ = ilist;
  }

  /* Allocator */
  allocator_type get_allocator() const { return alloc_; }

  /* Element access */
  T& at(const Key& key) {
    iterator it = container_.find(key);
    if (it == container_.end()) throw std::out_of_range{"Key not found"};
    return it->second;
  }

  const T& at(const Key& key) const {
    auto it = container_.find(key);
    if (it == container_.end()) throw std::out_of_range{"Key not found"};
    return it->second;
  }

  T& operator[](const Key& key) {
    auto it = container_.find(key);
    if (it != container_.end()) return it->second;
    return container_.emplace(key, mapped_type{}).first->second;
  }

  T& operator[](Key&& key) {
    auto it = container_.find(key);
    if (it != container_.end()) return it->second;
    return container_.emplace(std::move(key), mapped_type{}).first->second;
  }

  /* Iterators */
  iterator begin() noexcept { return container_.begin(); }
  iterator end() noexcept { return container_.end(); }
  const_iterator begin() const noexcept { return container_.begin(); }
  const_iterator end() const noexcept { return container_.end(); }
  const_iterator cbegin() const noexcept { return container_.cbegin(); }
  const_iterator cend() const noexcept { return container_.cend(); }
  reverse_iterator rbegin() noexcept { return container_.rbegin(); }
  reverse_iterator rend() noexcept { return container_.rend(); }
  const_reverse_iterator rbegin() const noexcept { return container_.rbegin(); }
  const_reverse_iterator rend() const noexcept { return container_.rend(); }
  const_reverse_iterator crbegin() const noexcept {
    return container_.crbegin();
  }
  const_reverse_iterator crend() const noexcept { return container_.crend(); }

  /* Capacity */
  bool empty() const { return container_.empty(); }
  size_type size() const { return container_.size(); }
  size_type max_size() const { return container_.max_size(); }

  /* Modifiers */
  void clear() { container_.clear(); }

  std::pair<iterator, bool> insert(const value_type& value) {
    return container_.insert(value);
  }

  std::pair<iterator, bool> insert(value_type&& value) {
    return container_.insert(std::move(value));
  }

  iterator insert(const_iterator hint, const value_type& value) {
    return container_.insert(hint, value);
  }

  iterator insert(const_iterator hint, value_type&& value) {
    return container_.insert(hint, std::move(value));
  }

  template <class InputIt>
  void insert(InputIt first, InputIt last) {
    while (first != last) insert(*first++);
  }

  void insert(std::initializer_list<value_type> ilist) {
    for (auto e : ilist) insert(std::move(e));
  }

  insert_return_type insert(node_type&& nh);

  iterator insert(const_iterator hint, node_type&& nh);

  template <class... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return container_.emplace(std::forward<Args>(args)...);
  }

  template <class... Args>
  std::pair<iterator, bool> try_emplace(const key_type& k, Args&&... args) {
    try_emplace(end(), k, std::forward<Args>(args)...);
  }

  template <class... Args>
  std::pair<iterator, bool> try_emplace(key_type&& k, Args&&... args) {
    try_emplace(end(), std::move(k), std::forward<Args>(args)...);
  }

  template <class... Args>
  iterator try_emplace(const_iterator hint, const key_type& k, Args&&... args) {
    auto it = container_.find(k);
    if (it != container_.end()) return {it, false};
    return emplace_hint(hint, std::piecewise_construct,
                        std::forward_as_tuple(k),
                        std::forward_as_tuple(std::forward<Args>(args)...));
  }

  template <class... Args>
  iterator try_emplace(const_iterator hint, key_type&& k, Args&&... args) {
    auto it = container_.find(k);
    if (it != container_.end()) return {it, false};
    return emplace_hint(hint, std::piecewise_construct,
                        std::forward_as_tuple(std::move(k)),
                        std::forward_as_tuple(std::forward<Args>(args)...));
  }

  iterator erase(const_iterator pos) { return container_.erase(pos); }

  iterator erase(iterator pos) { return container_.erase(pos); }

  iterator erase(const_iterator first, const_iterator last) {
    return container_.erase(first, last);
  }

  size_type erase(const key_type& key) { return container_.erase(key); }

  void swap(map& other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value&&
          std::__is_nothrow_swappable<Compare>::value) {
    std::swap(val_comp_, other.val_comp_);
    std::swap(container_, other.container_);
  }

  node_type extract(const_iterator position) {
    return container_.extract(position);
  }
  node_type extract(const key_type& x) {
    auto it = container_.find(x);
    if (it != container_.end()) return extract(it);
    return node_type{};
  }

  template <class C2>
  void merge(map<Key, T, C2, Allocator>& source) {
    return merge(std::move(source));
  }

  template <class C2>
  void merge(map<Key, T, C2, Allocator>&& source) {
    return container_.merge(std::move(source.container_));
  }

  /* Lookup */

  template <class K>
  size_type count(const K& x) const {
    if (find(x) != end()) return 1;
    return 0;
  }

  template <class K>
  iterator find(const K& x) {
    return container_.find(x);
  }

  template <class K>
  const_iterator find(const K& x) const {
    return container_.find(x);
  }

  template <class K>
  std::pair<iterator, iterator> equal_range(const K& x) {
    auto it = find(x);
    if (it != end()) {
      auto next = it;
      ++next;
      return {it, next};
    }
    return {it, it};
  }

  template <class K>
  std::pair<const_iterator, const_iterator> equal_range(const K& x) const {
    auto lower = find(x);
    if (lower != end()) {
      auto upper = lower;
      ++upper;
      return {lower, upper};
    }
    return {lower, lower};
  }

  template <class K>
  iterator lower_bound(const K& x) {
    return find(x);
  }

  template <class K>
  const_iterator lower_bound(const K& x) const {
    return find(x);
  }

  template <class K>
  iterator upper_bound(const K& x) {
    auto it = find(x);
    if (it != end()) return ++it;
    return it;
  }

  template <class K>
  const_iterator upper_bound(const K& x) const {
    auto it = find(x);
    if (it != end()) return ++it;
    return it;
  }

  /* Observers */

  key_compare key_comp() const { return comp_; }

  value_compare value_comp() const { return val_comp_; }

 private:
  key_compare comp_;
  value_compare val_comp_;
  allocator_type alloc_;
  container_type container_;
};
}  // namespace wijagels

#endif  // INCLUDE_MAP_HPP_
