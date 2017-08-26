// Copyright 2017 William Jagels
#ifndef INCLUDE_SKIPLIST_HPP_
#define INCLUDE_SKIPLIST_HPP_
#include <cassert>
#include <functional>
#include <iterator>
#include <memory>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

#ifndef DNDEBUG
#include <iostream>
#endif

namespace wijagels {
namespace detail {
template <class Iterator, class NodeType>
struct InsertReturnType {
  Iterator position;
  bool inserted;
  NodeType node;
};
}  // namespace detail

static std::geometric_distribution<size_t> distrib;
static std::random_device rd;

template <typename T, class Compare = std::less<T>,
          class Allocator = std::allocator<T>>
class skiplist {
  struct skip_node_base {
    explicit skip_node_base(size_t level) noexcept
        : d_prevs{level, this}, d_nexts{level, this} {}

    skip_node_base(std::vector<skip_node_base *> prevs,
                   std::vector<skip_node_base *> nexts) noexcept
        : d_prevs{prevs}, d_nexts{nexts} {}

    skip_node_base(const skip_node_base &other) = default;

    skip_node_base(skip_node_base &&other) noexcept
        : d_prevs{std::move(other.d_prevs)}, d_nexts{std::move(other.d_nexts)} {
      other.d_nexts = {&other};
      other.d_prevs = {&other};
    }

    ~skip_node_base() = default;

    skip_node_base &operator=(const skip_node_base &other) = default;

    skip_node_base &operator=(skip_node_base &&other) noexcept {
      if (&other == this) return *this;
      d_prevs = std::move(other.d_prevs);
      d_nexts = std::move(other.d_nexts);
      other.d_nexts = {&other};
      other.d_prevs = {&other};
      return *this;
    }

    size_t links() const noexcept { return d_prevs.size(); }

    inline void expand(size_t size) {
      if (links() < size) {
        d_prevs.resize(size, this);
        d_nexts.resize(size, this);
      }
    }

    friend void swap(skip_node_base &lhs, skip_node_base &rhs) noexcept {
      std::swap(std::move(lhs.d_prevs), std::move(rhs.d_prevs));
      std::swap(std::move(lhs.d_nexts), std::move(rhs.d_nexts));
    }

    std::vector<skip_node_base *> d_prevs, d_nexts;
  };

  struct skip_node : public skip_node_base {
    skip_node(std::vector<skip_node_base *> prevs,
              std::vector<skip_node_base *> nexts, const T &data)
        : skip_node_base{prevs, nexts}, d_data{data} {}

    explicit skip_node(size_t level, const T &data)
        : skip_node_base{level}, d_data{data} {}

    explicit skip_node(size_t level, T &&data)
        : skip_node_base{level}, d_data{std::move(data)} {}

    template <typename... Args>
    explicit skip_node(size_t level, Args &&... args)
        : skip_node_base{level}, d_data{std::forward<Args>(args)...} {}

    friend void swap(skip_node &lhs, skip_node &rhs) noexcept {
      std::swap(lhs.d_data, rhs.d_data);
      std::swap(lhs.d_prevs, rhs.d_prevs);
      std::swap(lhs.d_nexts, rhs.d_nexts);
    }

    T d_data;
  };

 public:
  class const_iterator;
  class iterator {
    friend skiplist;
    skip_node_base *d_node_p;
    size_t d_level = 0;

   public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = size_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer =
        typename std::allocator_traits<Allocator>::const_pointer;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit iterator(skip_node_base *node, size_t level = 0)
        : d_node_p{node}, d_level{level} {}

   private:
    explicit iterator(const const_iterator &other)
        : iterator{other.un_const()} {}

   public:
    iterator &operator++() {
      d_node_p = d_node_p->d_nexts[d_level];
      return *this;
    }

    iterator operator++(int) {
      auto ret = iterator{*this};
      d_node_p = d_node_p->d_nexts[d_level];
      return ret;
    }

    iterator &operator--() {
      d_node_p = d_node_p->d_prevs[d_level];
      return *this;
    }

    iterator operator--(int) {
      auto ret = iterator{*this};
      d_node_p = d_node_p->d_prevs[d_level];
      return ret;
    }

    iterator &down() {
      --d_level;
      return *this;
    }

    iterator &up() {
      if (d_level + 1 < d_node_p->links()) ++d_level;
      return *this;
    }

    reference operator*() const {
      return static_cast<skip_node *>(d_node_p)->d_data;
    }

    pointer operator->() const {
      return &(static_cast<skip_node *>(d_node_p)->d_data);
    }

    friend bool operator==(const iterator &lhs, const iterator &rhs) {
      return lhs.d_node_p == rhs.d_node_p;
    }

    friend bool operator!=(const iterator &lhs, const iterator &rhs) {
      return lhs.d_node_p != rhs.d_node_p;
    }

    friend difference_type distance(const iterator &lhs, const iterator &rhs) {
      size_type d = 0;
      auto it = lhs;
      while (it++ != rhs) d++;
      return d;
    }
  };

  class const_iterator {
    friend skiplist;
    friend iterator;
    const skip_node_base *d_node_p;
    size_t d_level;

   public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = size_t;
    using reference = const value_type &;
    using pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit const_iterator(const skip_node_base *node, size_t level = 0)
        : d_node_p{node}, d_level{level} {}

    const_iterator(const iterator &other)
        : d_node_p{other.d_node_p}, d_level{other.d_level} {}

    const_iterator &operator++() {
      d_node_p = d_node_p->d_nexts[d_level];
      return *this;
    }

    const_iterator operator++(int) {
      auto ret = const_iterator{*this};
      d_node_p = d_node_p->d_nexts[d_level];
      return ret;
    }

    const_iterator &operator--() {
      d_node_p = d_node_p->d_prevs[d_level];
      return *this;
    }

    const_iterator operator--(int) {
      auto ret = const_iterator{*this};
      d_node_p = d_node_p->d_prevs[d_level];
      return ret;
    }

    const_iterator &down() {
      --d_level;
      return *this;
    }

    const_iterator &up() {
      if (d_level + 1 < d_node_p->links()) ++d_level;
      return *this;
    }

    reference operator*() const {
      return static_cast<const skip_node *>(d_node_p)->d_data;
    }

    pointer operator->() const {
      return &(static_cast<const skip_node *>(d_node_p)->d_data);
    }

    friend bool operator==(const const_iterator &lhs,
                           const const_iterator &rhs) {
      return lhs.d_node_p == rhs.d_node_p;
    }

    friend bool operator!=(const const_iterator &lhs,
                           const const_iterator &rhs) {
      return lhs.d_node_p != rhs.d_node_p;
    }

    friend difference_type distance(const const_iterator &lhs,
                                    const const_iterator &rhs) {
      difference_type d = 0;
      auto it = lhs;
      while (it++ != rhs) d++;
      return d;
    }

   private:
    iterator un_const() const {
      return iterator{const_cast<skip_node_base *>(d_node_p)};
    }
  };

 public:
  using value_type = T;
  using value_compare = Compare;
  using allocator_type = Allocator;
  using size_type = size_t;
  using difference_type = size_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = typename std::allocator_traits<allocator_type>::pointer;
  using const_pointer =
      typename std::allocator_traits<allocator_type>::const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using node_ptr = skip_node *;
  using insert_type = std::pair<iterator, bool>;
  using node_allocator_type = typename std::allocator_traits<
      allocator_type>::template rebind_alloc<skip_node>;
  using node_alloc_traits = typename std::allocator_traits<node_allocator_type>;
  class node_type;
  using insert_return_type = detail::InsertReturnType<iterator, node_type>;

  class node_type {
   public:
    using allocator_type = node_allocator_type;

   protected:
    friend skiplist;
    node_ptr d_node_p;
    allocator_type d_alloc;

    inline void destroy_() {
      if (d_node_p) {
        std::allocator_traits<allocator_type>::destroy(d_alloc, d_node_p);
        std::allocator_traits<allocator_type>::deallocate(d_alloc, d_node_p, 1);
      }
    }

    /*
     * Releases ownership over the node pointer and returns an owning pointer
     */
    node_ptr release_() {
      auto ptr = d_node_p;
      d_node_p = nullptr;
      return ptr;
    }

   public:
    constexpr node_type() : node_type{nullptr} {}

    node_type(node_ptr ptr, allocator_type alloc = allocator_type{})
        : d_node_p{ptr}, d_alloc{std::move(alloc)} {}

    node_type(const node_type &) = delete;

    node_type(node_type &&other)
        : node_type{other.d_node_p, std::move(other.d_alloc)} {
      other.d_node_p = nullptr;
    }

    ~node_type() { destroy_(); }

    node_type &operator=(const node_type &) = delete;

    node_type &operator=(node_type &&other) {
      destroy_();
      d_node_p = other.d_node_p;
      other.d_node_p = nullptr;
      if (typename std::allocator_traits<
              allocator_type>::propagate_on_container_move_assignment())
        d_alloc = std::move(other.d_alloc);
      return *this;
    }

    bool empty() const noexcept { return d_node_p == nullptr; }

    explicit operator bool() const noexcept { return !empty(); }

    allocator_type get_allocator() const noexcept { return d_alloc; }

    void swap(node_type &nh) noexcept(
        std::allocator_traits<
            allocator_type>::propogate_on_container_swap::value ||
        std::allocator_traits<allocator_type>::is_always_equal::value) {
      if (!(empty() && nh.empty())) {  // Swapping two empties is a no-op
        if (std::allocator_traits<
                allocator_type>::propogate_on_container_swap::value) {
          std::swap(d_alloc, nh.d_alloc);
        }
        std::swap(d_node_p, nh.d_node_p);
      }
    }

    friend void swap(node_type &x, node_type &y) noexcept { x.swap(y); }
  };

 private:
  constexpr void link_(size_t level, skip_node_base *first,
                       skip_node_base *last) {
    first->d_nexts[level] = last;
    last->d_prevs[level] = first;
  }

  template <typename... Args>
  constexpr void link_(size_t level, skip_node_base *first,
                       skip_node_base *second, Args... rest) {
    link_(level, first, second);
    link_(level, second, rest...);
  }

  /*
   * Return an iterator directly after the location where some data should be
   * inserted.
   * If the data is already present, return {iter, false} where iter is the
   * iterator pointing to that data
   */
  template <class K>
  insert_type find_pos_(iterator hint, const K &data) {
    if (begin() == end()) return {end(), true};
    if (hint == end() || d_comp(*hint, data)) {  // Go forwards
      hint.d_level = hint.d_node_p->links() - 1;
      while (hint.d_level > 0) {
        auto next = hint;
        ++next;
        if (next == end()) {
          hint.down();
        } else if (d_comp(*next, data)) {
          ++hint;
        } else if (d_comp(data, *next)) {
          hint.down();
        } else {
          next.d_level = 0;
          return {next, false};
        }
      }
      if (hint == end()) ++hint;
      while (hint != end() && d_comp(*hint, data)) {
        ++hint;
      }
      /*!d_comp(*hint, data) &&*/
      if (hint != end() && !d_comp(data, *hint)) return {hint, false};
      return {hint, true};
    } else if (d_comp(data, *hint)) {  // Go backwards
      hint.d_level = hint.d_node_p->links() - 1;
      while (hint.d_level > 0) {
        auto prev = hint;
        --prev;
        if (prev == end()) {
          hint.down();
        } else if (d_comp(data, *hint)) {
          --hint;
        } else if (d_comp(*hint, data)) {
          hint.down();
        } else {
          prev.d_level = 0;
          return {prev, false};
        }
      }
      if (hint == end()) --hint;
      while (hint != end() && d_comp(data, *hint)) {
        --hint;
      }
      ++hint;
      if (!d_comp(*hint, data) && !d_comp(data, *hint)) return {hint, false};
      return {hint, true};
    }
    hint.d_level = 0;
    return {hint, false};
  }

  /*
   * Inserts the node directly before loc
   */
  void insert_node_(const iterator &loc, skip_node_base *node) {
    size_t lvl = node->links();
    d_head.expand(lvl);
    auto cur = loc;
    cur.d_level = 0;
    --cur;
    for (size_t i = 0; i < lvl; i++) {
      while (cur.d_level < i) {
        while (cur.d_node_p->links() <= i) {
          --cur;
        }
        cur.up();
      }
      assert(cur.d_level == i);
      assert(i < cur.d_node_p->links());
      assert(i < cur.d_node_p->d_nexts.size());
      link_(i, cur.d_node_p, node, cur.d_node_p->d_nexts[i]);
    }
  }

  void unlink_node_(skip_node_base *node) {
    for (size_t i = 0; i < node->links(); i++) {
      link_(i, node->d_prevs[i], node->d_nexts[i]);
    }
  }

  template <typename... Args>
  auto allocate_node_(Args &&... args) {
    auto node = node_alloc_traits::allocate(d_node_alloc, 1);
    if (!node) {
      throw std::bad_alloc{};
    }
    try {
      node_alloc_traits::construct(d_node_alloc, node,
                                   std::forward<Args>(args)...);
    } catch (...) {
      node_alloc_traits::deallocate(d_node_alloc, node, 1);
      throw;
    }
    return node;
  }

  void destroy_node_(skip_node_base *node) {
    destroy_node_(static_cast<node_ptr>(node));
  }

  void destroy_node_(node_ptr node) {
    node_alloc_traits::destroy(d_node_alloc, node);
    node_alloc_traits::deallocate(d_node_alloc, node, 1);
  }

 public:
  skiplist() : skiplist{value_compare{}} {}

  explicit skiplist(const value_compare &cmp,
                    const allocator_type &alloc = allocator_type{})
      : d_comp{cmp}, d_alloc{alloc}, d_head{1} {}

  skiplist(const skiplist &other)
      : d_comp{other.d_comp}, d_alloc{other.d_alloc}, d_head{1} {
    auto it = end();
    for (auto &e : other) {
      it = insert(it, e).first;
    }
  }

  skiplist(const skiplist &&other)
      : d_comp{std::move(other.d_comp)},
        d_alloc{std::move(other.d_alloc)},
        d_head{std::move(other.d_head)} {}

  skiplist(std::initializer_list<value_type> init,
           const value_compare &cmp = value_compare(),
           const Allocator &alloc = Allocator())
      : skiplist{cmp, alloc} {
    for (auto &&e : init) {
      emplace(std::move(e));
    }
  }

  ~skiplist() { clear(); }

  skiplist &operator=(const skiplist &other) {
    if (this == &other) return *this;
    clear();
    for (auto &e : other) {
      insert(e);
    }
    return *this;
  }

  skiplist &operator=(skiplist &&other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value
          &&std::is_nothrow_move_assignable<Compare>::value) {
    clear();
    if (typename std::allocator_traits<
            allocator_type>::propagate_on_container_move_assignment()) {
      d_alloc = other.d_alloc;
    }
    if (d_alloc != other.d_alloc) {
      for (auto e : other) {
        insert(e);
      }
    } else {
      d_head = std::move(other.d_head);
      other.d_head.d_nexts = {{&other.d_head}};
      other.d_head.d_prevs = {{&other.d_head}};
      assert(other.empty());
    }
    return *this;
  }

  /* Iterators */
  iterator begin() noexcept { return ++end(); }
  iterator end() noexcept { return iterator{&d_head}; }
  const_iterator begin() const noexcept { return cbegin(); }
  const_iterator end() const noexcept { return cend(); }
  const_iterator cbegin() const noexcept { return ++cend(); }
  const_iterator cend() const noexcept { return const_iterator{&d_head}; }
  reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }
  reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }
  const_reverse_iterator rbegin() const noexcept { return crbegin(); }
  const_reverse_iterator rend() const noexcept { return crend(); }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator{cend()};
  }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator{cbegin()};
  }

  /* Capacity */
  bool empty() const noexcept { return begin() == end(); }

  size_type size() const noexcept { return std::distance(begin(), end()); }

  size_type max_size() const noexcept { return node_alloc_traits::max_size(); }

  /* Modifiers */
  void clear() {
    /*
     * Don't bother fixing links if we're destroying the entire list
     */
    if (begin() != end()) {
      auto last = begin();
      for (auto it = ++begin(); it != end(); it++) {
        destroy_node_(last.d_node_p);
        last = it;
      }
      destroy_node_(last.d_node_p);
      d_head.d_nexts = {{&d_head}};
      d_head.d_prevs = {{&d_head}};
    }
  }

  insert_type insert(const_reference data) { return insert(end(), data); }

  insert_type insert(const_iterator hint, const_reference data) {
    auto pos = find_pos_(iterator{hint}, data);
    if (!pos.second) return pos;

    size_t lvl = distrib(d_gen) + 1;
    node_ptr node = allocate_node_(lvl, data);
    insert_node_(pos.first, node);
    return {iterator{node}, true};
  }

  insert_type insert(value_type &&data) {
    return insert(cend(), std::move(data));
  }

  insert_type insert(const_iterator hint, value_type &&data) {
    auto pos = find_pos_(iterator{hint}, data);
    if (!pos.second) return pos;

    size_t lvl = distrib(d_gen) + 1;
    node_ptr node = allocate_node_(lvl, std::move(data));
    insert_node_(pos.first, node);
    return {iterator{node}, true};
  }

  insert_return_type insert(node_type &&nh) {
    insert_return_type ret{end(), false, {}};
    if (!nh) {
      return ret;
    }
    auto pos = find_pos_(end(), nh.d_node_p->d_data);
    if (!pos.second) {
      printf("Pos exists\n");
      ret.position = pos.first;
      return ret;
    }
    size_t lvl = distrib(d_gen) + 1;
    auto node = nh.release_();
    node->expand(lvl);
    insert_node_(pos.first, node);
    ret.inserted = true;
    ret.position = iterator{node};
    return ret;
  }

  iterator insert(const_iterator hint, node_type &&nh) {
    if (!nh) return end();
    size_t lvl = distrib(d_gen) + 1;
    auto pos = find_pos_(iterator{hint}, nh.d_node_p->d_data);
    if (!pos.second) {
      return pos.first;
    }
    auto node = nh.release_();
    node->expand(lvl);
    insert_node_(pos.first, node);
    return iterator{node};
  }

  template <typename... Args>
  insert_type emplace(Args &&... args) {
    auto data = value_type{std::forward<Args>(args)...};
    auto pos = find_pos_(end(), data);
    if (!pos.second) {
      return pos;
    }
    size_t lvl = distrib(d_gen) + 1;
    node_ptr node = allocate_node_(lvl, std::move(data));
    insert_node_(pos.first, node);
    return {iterator{node}, true};
  }

  template <typename... Args>
  iterator emplace_hint(const_iterator hint, Args &&... args) {
    auto data = value_type{std::forward<Args>(args)...};
    auto pos = find_pos_(iterator{hint}, data);
    if (!pos.second) {
      return pos.first;
    }
    size_t lvl = distrib(d_gen) + 1;
    node_ptr node = allocate_node_(lvl, std::move(data));
    insert_node_(pos.first, node);
    return iterator{node};
  }

  iterator erase(const iterator &pos) {
    auto ret = iterator{pos.d_node_p->d_nexts[0]};
    auto node = pos.d_node_p;
    unlink_node_(node);
    destroy_node_(node);
    return ret;
  }

  template <typename K>
  size_type erase(const K &val) {
    auto it = find(val);
    if (it != end()) {
      erase(it);
      return 1;
    }
    return 0;
  }

  void swap(skiplist &other) noexcept(
      std::allocator_traits<Allocator>::is_always_equal::value
          &&std::__is_nothrow_swappable<Compare>::value) {
    std::swap(d_comp, other.d_comp);
    std::swap(d_alloc, other.d_alloc);
  }

  node_type extract(const const_iterator &pos) {
    auto node = static_cast<node_ptr>(pos.un_const().d_node_p);
    unlink_node_(node);
    return node_type{node, d_node_alloc};
  }

  node_type extract(const_reference data) {
    auto res = find_pos_(end(), data);
    if (!res.second) return extract(res.first);
    return node_type{};
  }

  template <class C2>
  void merge(skiplist<T, C2, Allocator> &source) {
    merge(std::move(source));
  }

  template <class C2>
  void merge(skiplist<T, C2, Allocator> &&source) {
    for (auto it = source.begin(); it != source.end();) {
      auto r = find_pos_(end(), *it);
      auto node = it.d_node_p;
      ++it;
      if (r.second) {
        source.unlink_node_(node);
        insert_node_(r.first, node);
      }
    }
  }

  /* Lookup */
  template <class K>
  iterator find(const K &data) {
    auto r = find_pos_(end(), data);
    if (!r.second) return r.first;
    return end();
  }

  template <class K>
  const_iterator find(const K &data) const {
    auto r = find_pos_(end(), data);
    if (!r.second) return r.first;
    return end();
  }

  /* Observers */
  value_compare value_comp() const { return d_comp; }

#ifdef DNDEBUG
  /*
   * Utilities
   * Not for normal use
   */
  void print_state() {
    for (size_t i = 0; i < d_head.links(); i++) {
      auto iter = const_iterator{d_head.d_nexts[i], i};
      while (iter != cend()) {
        std::cout << *iter << ", ";
        ++iter;
      }
      std::cout << "END\n";
    }
  }

  void verify_integrity() {
    for (size_t i = 0; i < d_head.links(); i++) {
      auto iter = const_iterator{d_head.d_nexts[i], i};
      while (iter != cend()) {
        auto old = iter++;
        assert(old == --iter);
        ++iter;
      }
    }
  }
#else
  void print_state() {}
  void verify_integrity() {}
#endif

 private:
  value_compare d_comp;
  allocator_type d_alloc;
  node_allocator_type d_node_alloc;
  skip_node_base d_head;
  std::mt19937 d_gen{rd()};
};

template <class T, class Compare, class Alloc>
bool operator==(const skiplist<T, Compare, Alloc> &lhs,
                const skiplist<T, Compare, Alloc> &rhs) {
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                    lhs.value_comp());
}

template <class T, class Compare, class Alloc>
bool operator!=(const skiplist<T, Compare, Alloc> &lhs,
                const skiplist<T, Compare, Alloc> &rhs) {
  return !(lhs == rhs);
}

template <class T, class Compare, class Alloc>
bool operator<(const skiplist<T, Compare, Alloc> &lhs,
               const skiplist<T, Compare, Alloc> &rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end(), lhs.value_comp());
}

template <class T, class Compare, class Alloc>
bool operator<=(const skiplist<T, Compare, Alloc> &lhs,
                const skiplist<T, Compare, Alloc> &rhs) {
  return (lhs < rhs) || (lhs == rhs);
}

template <class T, class Compare, class Alloc>
bool operator>(const skiplist<T, Compare, Alloc> &lhs,
               const skiplist<T, Compare, Alloc> &rhs) {
  return rhs < lhs;
}

template <class T, class Compare, class Alloc>
bool operator>=(const skiplist<T, Compare, Alloc> &lhs,
                const skiplist<T, Compare, Alloc> &rhs) {
  return rhs <= lhs;
}

}  // namespace wijagels

#endif  // INCLUDE_SKIPLIST_HPP_
