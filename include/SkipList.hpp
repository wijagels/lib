// Copyright 2017 William Jagels
#ifndef INCLUDE_SKIPLIST_HPP_
#define INCLUDE_SKIPLIST_HPP_
#include <cassert>
#include <functional>
#include <memory>
#include <random>
#include <utility>
#include <vector>

namespace wijagels {
static std::geometric_distribution<size_t> distrib;
static std::random_device rd;

template <typename T, class Compare = std::less<T>,
          class Allocator = std::allocator<T>>
class skiplist {
  struct skip_node_base {
    explicit skip_node_base(size_t level)
        : prevs_{level, this}, nexts_{level, this} {}

    skip_node_base(std::vector<skip_node_base *> prevs,
                   std::vector<skip_node_base *> nexts)
        : prevs_{prevs}, nexts_{nexts} {}

    inline size_t links() const { return prevs_.size(); }

    inline void expand(size_t size) {
      if (links() < size) {
        prevs_.resize(size, this);
        nexts_.resize(size, this);
      }
    }

    friend void swap(skip_node_base &lhs, skip_node_base &rhs) noexcept {
      std::swap(lhs.prevs_, rhs.prevs_);
      std::swap(lhs.nexts_, rhs.nexts_);
    }

    std::vector<skip_node_base *> prevs_, nexts_;
  };

  struct skip_node : skip_node_base {
    skip_node(std::vector<skip_node_base *> prevs,
              std::vector<skip_node_base *> nexts, const T &data)
        : skip_node_base{prevs, nexts}, data_{data} {}

    explicit skip_node(const T &data) : skip_node{1lu, data} {}

    explicit skip_node(size_t level, const T &data)
        : skip_node_base{level}, data_{data} {}

    template <typename... Args>
    explicit skip_node(size_t level, Args &&... args)
        : skip_node_base{level}, data_{args...} {}

    friend void swap(skip_node &lhs, skip_node &rhs) noexcept {
      std::swap(lhs.data_, rhs.data_);
      std::swap(lhs.prevs_, rhs.prevs_);
      std::swap(lhs.nexts_, rhs.nexts_);
    }

    T data_;
  };

 public:
  class iterator {
    friend skiplist;
    skip_node_base *node_;
    size_t level_ = 0;

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
        : node_{node}, level_{level} {}

    iterator &operator++() {
      node_ = node_->nexts_[level_];
      return *this;
    }

    iterator operator++(int) {
      iterator ret = iterator{*this};
      node_ = node_->nexts_[level_];
      return ret;
    }

    iterator &operator--() {
      node_ = node_->prevs_[level_];
      return *this;
    }

    iterator operator--(int) {
      iterator ret = iterator{*this};
      node_ = node_->prevs_[level_];
      return ret;
    }

    iterator &down() {
      --level_;
      return *this;
    }

    iterator &up() {
      if (level_ + 1 < node_->links()) ++level_;
      return *this;
    }

    reference operator*() const {
      return static_cast<skip_node *>(node_)->data_;
    }

    pointer operator->() const {
      return &(static_cast<skip_node *>(node_)->data_);
    }

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
    friend skiplist;
    const skip_node_base *node_;
    size_t level_;

   public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = size_t;
    using reference = const value_type &;
    using pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit const_iterator(const skip_node_base *node, size_t level = 0)
        : node_{node}, level_{level} {}

    const_iterator(const iterator &other)
        : node_{other.node_}, level_{other.level_} {}

    const_iterator &operator++() {
      node_ = node_->nexts_[level_];
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator ret = const_iterator{*this};
      node_ = node_->nexts_[level_];
      return ret;
    }

    const_iterator &operator--() {
      node_ = node_->prevs_[level_];
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator ret = const_iterator{*this};
      node_ = node_->prevs_[level_];
      return ret;
    }

    const_iterator &down() {
      --level_;
      return *this;
    }

    const_iterator &up() {
      if (level_ + 1 < node_->links()) ++level_;
      return *this;
    }

    reference operator*() const {
      return static_cast<const skip_node *>(node_)->data_;
    }

    pointer operator->() const {
      return &(static_cast<const skip_node *>(node_)->data_);
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
    iterator un_const() const {
      return iterator{const_cast<skip_node_base *>(node_)};
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
  using node_type = std::shared_ptr<skip_node>;
  using insert_type = std::pair<bool, iterator>;
  using node_allocator_type = typename std::allocator_traits<
      allocator_type>::template rebind_alloc<skip_node>;
  using node_alloc_traits = typename std::allocator_traits<node_allocator_type>;

 private:
  inline void link_(size_t level, skip_node_base *first, skip_node_base *last) {
    first->nexts_[level] = last;
    last->prevs_[level] = first;
  }

  template <typename... Args>
  inline void link_(size_t level, skip_node_base *first, skip_node_base *second,
                    Args... rest) {
    link_(level, first, second);
    link_(level, second, rest...);
  }

  /*
   * Return an iterator directly after the location where some data should be
   * inserted.
   * If the data is already present, return {false, iter} where iter is the
   * iterator pointing to that data
   */
  template <class K>
  insert_type find_pos_(iterator hint, const K &data) {
    if (begin() == end()) return {true, end()};
    if (hint == end() || comp(*hint, data)) {  // Go forwards
      hint.level_ = hint.node_->links() - 1;
      while (hint.level_ > 0) {
        auto next = hint;
        ++next;
        if (next == end()) {
          hint.down();
        } else if (comp(*next, data)) {
          ++hint;
        } else if (comp(data, *next)) {
          hint.down();
        } else {
          next.level_ = 0;
          return {false, next};
        }
      }
      if (hint == end()) ++hint;
      while (hint != end() && comp(*hint, data)) {
        ++hint;
      }
      /*!comp(*hint, data) &&*/
      if (hint != end() && !comp(data, *hint)) return {false, hint};
      return {true, hint};
    } else if (comp(data, *hint)) {  // Go backwards
      hint.level_ = hint.node_->links() - 1;
      while (hint.level_ > 0) {
        auto prev = hint;
        --prev;
        if (prev == end()) {
          hint.down();
        } else if (comp(data, *hint)) {
          --hint;
        } else if (comp(*hint, data)) {
          hint.down();
        } else {
          prev.level_ = 0;
          return {false, prev};
        }
      }
      if (hint == end()) --hint;
      while (hint != end() && comp(data, *hint)) {
        --hint;
      }
      ++hint;
      if (!comp(*hint, data) && !comp(data, *hint)) return {false, hint};
      return {true, hint};
    }
    hint.level_ = 0;
    return {false, hint};
  }

  /*
   * Inserts the node directly before loc
   */
  void insert_node_(const iterator &loc, skip_node_base *node) {
    size_t lvl = node->links();
    head_.expand(lvl);
    auto cur = loc;
    cur.level_ = 0;
    --cur;
    for (size_t i = 0; i < lvl; i++) {
      while (cur.level_ < i) {
        while (cur.node_->links() <= i) {
          --cur;
        }
        cur.up();
      }
      assert(cur.level_ == i);
      assert(i < cur.node_->links());
      assert(i < cur.node_->nexts_.size());
      link_(i, cur.node_, node, cur.node_->nexts_[i]);
    }
  }

  void unlink_node_(skip_node_base *node) {
    for (size_t i = 0; i < node->links(); i++) {
      link_(i, node->prevs_[i], node->nexts_[i]);
    }
  }

  void destroy_node_(skip_node_base *node) {
    destroy_node_(static_cast<node_ptr>(node));
  }

  void destroy_node_(node_ptr node) {
    node_alloc_traits::destroy(node_alloc_, node);
    node_alloc_traits::deallocate(node_alloc_, node, 1);
  }

 public:
  skiplist() : skiplist{value_compare{}} {}

  explicit skiplist(const value_compare &cmp,
                    const Allocator &alloc = Allocator{})
      : comp{cmp}, alloc_{alloc}, head_{1lu} {}

  skiplist(std::initializer_list<value_type> init,
           const value_compare &cmp = value_compare(),
           const Allocator &alloc = Allocator())
      : skiplist{cmp, alloc} {
    for (auto &&e : init) {
      emplace(e);
    }
  }

  ~skiplist() { clear(); }

  /* Iterators */
  iterator begin() noexcept { return ++end(); }
  iterator end() noexcept { return iterator{&head_}; }
  const_iterator begin() const noexcept { return cbegin(); }
  const_iterator end() const noexcept { return cend(); }
  const_iterator cbegin() const noexcept { return ++cend(); }
  const_iterator cend() const noexcept { return const_iterator{&head_}; }
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

  /* Modifiers */
  void clear() {
    /*
     * Don't bother fixing links if we're destroying the entire list
     */
    using std::allocator_traits;
    if (begin() != end()) {
      auto last = begin();
      for (auto it = ++begin(); it != end(); it++) {
        destroy_node_(last.node_);
        last = it;
      }
      destroy_node_(last.node_);
    }
  }

  insert_type insert(const_reference data) { return insert(end(), data); }

  insert_type insert(const iterator &hint, const_reference data) {
    using std::allocator_traits;
    auto pos = find_pos_(hint, data);
    if (!pos.first) return pos;

    size_t lvl = distrib(gen) + 1;
    node_ptr node = node_alloc_traits::allocate(node_alloc_, 1);
    node_alloc_traits::construct(node_alloc_, node, lvl, data);
    insert_node_(pos.second, node);
    return {true, iterator{node}};
  }

  template <typename... Args>
  insert_type emplace(Args &&... args) {
    size_t level = head_.links() - 1;
    auto pos = end();
    pos.level_ = level;
    if (level == 0) pos = begin();
    return emplace_hint(pos, std::forward<Args>(args)...);
  }

  template <typename... Args>
  insert_type emplace_hint(const iterator &hint, Args &&... args) {
    size_t lvl = distrib(gen) + 1;
    node_ptr node = node_alloc_traits::allocate(node_alloc_, 1);
    node_alloc_traits::construct(node_alloc_, node, lvl,
                                 std::forward<Args>(args)...);
    auto pos = find_pos_(hint, node->data_);
    if (!pos.first) {
      node_alloc_traits::destroy(node_alloc_, node);
      node_alloc_traits::deallocate(node_alloc_, node, 1);
      return pos;
    }
    insert_node_(pos.second, node);
    return {true, iterator{node}};
  }

  iterator erase(const iterator &pos) {
    using std::allocator_traits;
    iterator ret = pos.node_->nexts_[0];
    auto node = pos.node_;
    unlink_node_(node);
    destroy_node_(node);
    return ret;
  }

  node_type extract(const const_iterator &pos) {
    node_ptr node = static_cast<node_ptr>(pos.un_const().node_);
    unlink_node_(node);
    node_allocator_type alloc = node_alloc_;
    auto deleter = [alloc](node_ptr ptr) mutable {
      node_alloc_traits::destroy(alloc, ptr);
      node_alloc_traits::deallocate(alloc, ptr, 1);
    };
    return node_type{node, deleter};
  }

  node_type extract(const_reference data) {
    auto res = find_pos_(end(), data);
    if (!res.first) return extract(res.second);
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
      auto node = it.node_;
      ++it;
      if (r.first) {
        source.unlink_node_(node);
        insert_node_(r.second, node);
      }
    }
  }

  /* Lookup */
  template <class K>
  iterator find(const K &data) {
    auto r = find_pos_(end(), data);
    if (!r.first) return r.second;
    return end();
  }

  template <class K>
  const_iterator find(const K &data) const {
    auto r = find_pos_(end(), data);
    if (!r.first) return r.second;
    return end();
  }

  /* Observers */
  value_compare value_comp() const { return comp; }

  /* Utilities */
  /* Not for normal use */
  void print_state() {
    for (size_t i = 0; i < head_.links(); i++) {
      auto iter = const_iterator{head_.nexts_[i], i};
      while (iter != cend()) {
        printf("%d, ", *iter);
        ++iter;
      }
      printf("END\n");
    }
  }

  void verify_integrity() {
    for (size_t i = 0; i < head_.links(); i++) {
      auto iter = const_iterator{head_.nexts_[i], i};
      while (iter != cend()) {
        auto old = iter++;
        assert(old == --iter);
        ++iter;
      }
    }
  }

 private:
  value_compare comp;
  allocator_type alloc_;
  node_allocator_type node_alloc_;
  skip_node_base head_;
  std::mt19937_64 gen{rd()};
};
}  // namespace wijagels

#endif  // INCLUDE_SKIPLIST_HPP_
