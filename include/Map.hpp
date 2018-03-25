// Copyright 2017 William Jagels
#pragma once
#include "SkipList.hpp"
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace wijagels {
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
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = typename std::allocator_traits<allocator_type>::pointer;
  /* Comparison functor for value_type */
  class value_compare {
   protected:
    friend map;
    constexpr explicit value_compare(Compare c) : d_comp{std::move(c)} {}
    Compare d_comp;

   public:
    constexpr bool operator()(const value_type &lhs,
                              const value_type &rhs) const {
      return d_comp(lhs.first, rhs.first);
    }
    constexpr bool operator()(const key_type &lhs,
                              const value_type &rhs) const {
      return d_comp(lhs, rhs.first);
    }
    constexpr bool operator()(const value_type &lhs,
                              const key_type &rhs) const {
      return d_comp(lhs.first, rhs);
    }
    constexpr bool operator()(const key_type &lhs, const key_type &rhs) const {
      return d_comp(lhs, rhs);
    }
  };
  using container_type = skiplist<value_type, value_compare, allocator_type>;
  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;
  using reverse_iterator = typename container_type::reverse_iterator;
  using Iterator = iterator;
  using ConstIterator = const_iterator;
  using ReverseIterator = reverse_iterator;
  using const_reverse_iterator =
      typename container_type::const_reverse_iterator;
  struct node_type;
  using insert_return_type = detail::InsertReturnType<iterator, node_type>;

  struct node_type : public container_type::node_type {
    using key_type = const Key;
    using mapped_type = T;

    node_type() = default;
    ~node_type() = default;
    node_type(const node_type &) = delete;
    node_type(node_type &&) = default;

    /*
     * Conversion constructor from base class
     */
    node_type(typename container_type::node_type &&nh) noexcept(
        std::is_nothrow_move_constructible_v<decltype(nh)>)
        : container_type::node_type{std::move(nh)} {}

    node_type &operator=(const node_type &) = delete;
    node_type &operator=(node_type &&) = default;

    key_type &key() const {
      return container_type::node_type::d_node_p->d_data->first;
    }

    mapped_type &mapped() const {
      return container_type::node_type::d_node_p->d_data->second;
    }
  };

  /* Member functions */
  /* Constructors */
  map() : map{Compare()} {}

  explicit map(const Compare &comp, const Allocator &alloc = Allocator{})
      : d_comp{comp}, d_val_comp{comp}, d_container{d_val_comp, alloc} {}

  explicit map(const Allocator &alloc) : map{Compare{}, alloc} {}

  template <class InputIterator>
  map(InputIterator first, InputIterator last, const Compare &comp = Compare(),
      const Allocator &alloc = Allocator())
      : map{comp, alloc} {
    insert(first, last);
  }

  template <class InputIterator>
  map(InputIterator first, InputIterator last, const Allocator &alloc)
      : map{first, last, Compare(), alloc} {}

  map(const map &other)
      : d_comp{other.d_comp},
        d_val_comp{other.d_val_comp},
        d_container{other.d_container} {}

  map(const map &other, const Allocator &alloc)
      : d_comp{other.d_comp},
        d_val_comp{other.d_val_comp},
        d_container{other.d_container, alloc} {}

  map(map &&other) noexcept(
      std::is_nothrow_move_constructible_v<Compare>
          &&std::is_nothrow_move_constructible_v<container_type>)
      : d_comp{std::move(other.d_comp)},
        d_val_comp{std::move(other.d_val_comp)},
        d_container{std::move(other.d_container)} {}

  map(map &&other, const Allocator &alloc) noexcept(
      std::is_nothrow_move_constructible_v<Compare>
          &&std::is_nothrow_move_constructible_v<container_type>)
      : d_comp{std::move(other.d_comp)},
        d_val_comp{std::move(other.d_val_comp)},
        d_container{std::move(other.d_container), alloc} {}

  map(std::initializer_list<value_type> init, const Compare &comp = Compare(),
      const Allocator &alloc = Allocator{})
      : map{comp, alloc} {
    insert(init);
  }

  map(std::initializer_list<value_type> init, const Allocator &alloc)
      : map{alloc} {
    insert(init);
  }

  /* Destructor */
  ~map() = default;

  /* Assignment */
  map &operator=(const map &other) {
    if (this == &other) return *this;
    clear();
    d_comp = other.d_comp;
    d_val_comp = value_compare{d_comp};
    d_container = other.d_container;
    return *this;
  }

  map &operator=(map &&other) noexcept(
      std::is_nothrow_move_assignable_v<container_type> && std::is_nothrow_move_assignable_v<Compare>) {
    d_val_comp = std::move(other.d_val_comp);
    d_comp = std::move(other.d_comp);
    d_container = std::move(other.d_container);
  }

  map &operator=(std::initializer_list<value_type> ilist) {
    d_container = ilist;
  }

  /* Allocator */
  allocator_type get_allocator() const { return d_container.get_allocator(); }

  /* Element access */
  T &at(const Key &key) {
    iterator it = d_container.find(key);
    if (it == d_container.end()) throw std::out_of_range{"Key not found"};
    return it->second;
  }

  const T &at(const Key &key) const {
    auto it = d_container.find(key);
    if (it == d_container.end()) throw std::out_of_range{"Key not found"};
    return it->second;
  }

  T &operator[](const Key &key) {
    auto it = d_container.find(key);
    if (it != d_container.end()) return it->second;
    return d_container.emplace(key, mapped_type{}).first->second;
  }

  T &operator[](Key &&key) {
    auto it = d_container.find(key);
    if (it != d_container.end()) return it->second;
    return d_container.emplace(std::move(key), mapped_type{}).first->second;
  }

  /* Iterators */
  iterator begin() noexcept { return d_container.begin(); }
  iterator end() noexcept { return d_container.end(); }
  const_iterator begin() const noexcept { return d_container.begin(); }
  const_iterator end() const noexcept { return d_container.end(); }
  const_iterator cbegin() const noexcept { return d_container.cbegin(); }
  const_iterator cend() const noexcept { return d_container.cend(); }
  reverse_iterator rbegin() noexcept { return d_container.rbegin(); }
  reverse_iterator rend() noexcept { return d_container.rend(); }
  const_reverse_iterator rbegin() const noexcept {
    return d_container.rbegin();
  }
  const_reverse_iterator rend() const noexcept { return d_container.rend(); }
  const_reverse_iterator crbegin() const noexcept {
    return d_container.crbegin();
  }
  const_reverse_iterator crend() const noexcept { return d_container.crend(); }

  /* Capacity */
  bool empty() const { return d_container.empty(); }
  size_type size() const { return d_container.size(); }
  size_type max_size() const { return d_container.max_size(); }

  /* Modifiers */
  void clear() { d_container.clear(); }

  std::pair<iterator, bool> insert(const value_type &value) {
    return d_container.insert(value);
  }

  std::pair<iterator, bool> insert(value_type &&value) {
    return d_container.insert(std::move(value));
  }

  iterator insert(const_iterator hint, const value_type &value) {
    return d_container.insert(hint, value);
  }

  iterator insert(const_iterator hint, value_type &&value) {
    return d_container.insert(hint, std::move(value));
  }

  template <class InputIt>
  void insert(InputIt first, InputIt last) {
    while (first != last) insert(*first++);
  }

  void insert(std::initializer_list<value_type> ilist) {
    for (auto e : ilist) insert(std::move(e));
  }

  insert_return_type insert(node_type &&nh) {
    auto container_return = d_container.insert(std::move(nh));

    insert_return_type ret{container_return.position, container_return.inserted,
                           std::move(container_return.node)};
    return ret;
  }

  iterator insert(const_iterator hint, node_type &&nh) {
    // Implicit upcast
    return d_container.insert(hint, std::move(nh));
  }

  template <class M>
  std::pair<iterator, bool> insert_or_assign(const key_type &k, M &&obj) {
    auto iter = find(k);
    if (iter == end()) {
      return emplace(k, std::forward<M>(obj));
    }
    iter->second = std::forward<M>(obj);
    return {iter, false};
  }

  template <class M>
  std::pair<iterator, bool> insert_or_assign(key_type &&k, M &&obj) {
    auto iter = find(k);
    if (iter == end()) {
      return emplace(std::move(k), std::forward<M>(obj));
    }
    iter->second = std::forward<M>(obj);
    return {iter, false};
  }

  template <class M>
  iterator insert_or_assign(const_iterator hint, const key_type &k, M &&obj) {
    auto iter = find(k);
    if (iter == end()) {
      return emplace_hint(hint, k, std::forward<M>(obj));
    }
    iter->second = std::forward<M>(obj);
    return iter;
  }

  template <class M>
  iterator insert_or_assign(const_iterator hint, key_type &&k, M &&obj) {
    auto iter = find(k);
    if (iter == end()) {
      return emplace_hint(hint, std::move(k), std::forward<M>(obj));
    }
    iter->second = std::forward<M>(obj);
    return iter;
  }

  template <class... Args>
  std::pair<iterator, bool> emplace(Args &&... args) {
    return d_container.emplace(std::forward<Args>(args)...);
  }

  template <class... Args>
  iterator emplace_hint(const_iterator hint, Args &&... args) {
    return d_container.emplace_hint(hint, std::forward<Args>(args)...);
  }

  template <class... Args>
  std::pair<iterator, bool> try_emplace(const key_type &k, Args &&... args) {
    auto it = d_container.find(k);
    if (it != d_container.end()) return {it, false};
    // TODO: optimize finding the spot to put the new element
    return emplace(std::piecewise_construct, std::forward_as_tuple(k),
                   std::forward_as_tuple(std::forward<Args>(args)...));
  }

  template <class... Args>
  std::pair<iterator, bool> try_emplace(key_type &&k, Args &&... args) {
    auto it = d_container.find(k);
    if (it != d_container.end()) return {it, false};
    // TODO: optimize finding the spot to put the new element
    return emplace(std::piecewise_construct,
                   std::forward_as_tuple(std::move(k)),
                   std::forward_as_tuple(std::forward<Args>(args)...));
  }

  template <class... Args>
  iterator try_emplace(const_iterator hint, const key_type &k,
                       Args &&... args) {
    auto it = d_container.find(k);
    if (it != d_container.end()) return it;
    // TODO: optimize finding the spot to put the new element
    return emplace_hint(hint, std::piecewise_construct,
                        std::forward_as_tuple(k),
                        std::forward_as_tuple(std::forward<Args>(args)...));
  }

  template <class... Args>
  iterator try_emplace(const_iterator hint, key_type &&k, Args &&... args) {
    auto it = d_container.find(k);
    if (it != d_container.end()) return it;
    // TODO: optimize finding the spot to put the new element
    return emplace_hint(hint, std::piecewise_construct,
                        std::forward_as_tuple(std::move(k)),
                        std::forward_as_tuple(std::forward<Args>(args)...));
  }

  iterator erase(const_iterator pos) { return d_container.erase(pos); }

  iterator erase(iterator pos) { return d_container.erase(pos); }

  iterator erase(const_iterator first, const_iterator last) {
    return d_container.erase(first, last);
  }

  size_type erase(const key_type &key) { return d_container.erase(key); }

  void swap(map &other) noexcept(
      container_type::swap(std::ref(container_type{}))) {
    std::swap(d_val_comp, other.d_val_comp);
    std::swap(d_container, other.d_container);
  }

  node_type extract(const_iterator position) {
    return d_container.extract(position);
  }

  node_type extract(const key_type &x) {
    auto it = d_container.find(x);
    if (it != d_container.end()) return extract(it);
    return node_type{};
  }

  template <class C2>
  void merge(map<Key, T, C2, Allocator> &source) {
    return merge(std::move(source));
  }

  template <class C2>
  void merge(map<Key, T, C2, Allocator> &&source) {
    return d_container.merge(std::move(source.d_container));
  }

  /* Lookup */

  template <class K>
  size_type count(const K &x) const {
    if (find(x) != end()) return 1;
    return 0;
  }

  template <class K>
  iterator find(const K &x) {
    return d_container.find(x);
  }

  template <class K>
  const_iterator find(const K &x) const {
    return d_container.find(x);
  }

  template <class K>
  std::pair<iterator, iterator> equal_range(const K &x) {
    auto it = find(x);
    if (it != end()) {
      auto next = it;
      ++next;
      return {it, next};
    }
    return {it, it};
  }

  template <class K>
  std::pair<const_iterator, const_iterator> equal_range(const K &x) const {
    auto lower = find(x);
    if (lower != end()) {
      auto upper = lower;
      ++upper;
      return {lower, upper};
    }
    return {lower, lower};
  }

  template <class K>
  iterator lower_bound(const K &x) {
    return find(x);
  }

  template <class K>
  const_iterator lower_bound(const K &x) const {
    return find(x);
  }

  template <class K>
  iterator upper_bound(const K &x) {
    auto it = find(x);
    if (it != end()) return ++it;
    return it;
  }

  template <class K>
  const_iterator upper_bound(const K &x) const {
    auto it = find(x);
    if (it != end()) return ++it;
    return it;
  }

  /* Observers */

  key_compare key_comp() const { return d_comp; }

  value_compare value_comp() const { return d_val_comp; }

 private:
  key_compare d_comp;
  value_compare d_val_comp;
  container_type d_container;
};

template <class Key, class T, class Compare, class Alloc>
bool operator==(const map<Key, T, Compare, Alloc> &lhs,
                const map<Key, T, Compare, Alloc> &rhs) {
  auto cmp = lhs.value_comp();
  auto first1 = lhs.begin();
  auto last1 = lhs.end();
  auto first2 = rhs.begin();
  auto last2 = rhs.end();
  for (; first1 != last1; ++first1, ++first2) {
    if (cmp(first1->first, first2->first) ||
        cmp(first2->first, first1->first) ||
        !(first1->second == first2->second)) {
      return false;
    }
  }
  return (first1 == last1) && (first2 == last2);
}

template <class Key, class T, class Compare, class Alloc>
bool operator!=(const map<Key, T, Compare, Alloc> &lhs,
                const map<Key, T, Compare, Alloc> &rhs) {
  return !(lhs == rhs);
}

template <class Key, class T, class Compare, class Alloc>
bool operator<(const map<Key, T, Compare, Alloc> &lhs,
               const map<Key, T, Compare, Alloc> &rhs) {
  auto cmp = lhs.value_comp();
  auto first1 = lhs.begin();
  auto last1 = lhs.end();
  auto first2 = rhs.begin();
  auto last2 = rhs.end();
  for (; first1 != last1; ++first1, ++first2) {
    if (cmp(first1->first, first2->first) || (first1->second < first2->second))
      return true;
    if (cmp(first2->first, first1->first) || first2->second < first1->second)
      return false;
  }
  return (first1 == last1) && (first2 != last2);
}

template <class Key, class T, class Compare, class Alloc>
bool operator<=(const map<Key, T, Compare, Alloc> &lhs,
                const map<Key, T, Compare, Alloc> &rhs) {
  return (lhs < rhs) || (lhs == rhs);
}

template <class Key, class T, class Compare, class Alloc>
bool operator>(const map<Key, T, Compare, Alloc> &lhs,
               const map<Key, T, Compare, Alloc> &rhs) {
  return rhs < lhs;
}
template <class Key, class T, class Compare, class Alloc>
bool operator>=(const map<Key, T, Compare, Alloc> &lhs,
                const map<Key, T, Compare, Alloc> &rhs) {
  return rhs <= lhs;
}

}  // namespace wijagels
