#define LIST_DBG_ON
#include "List.hpp"
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE list test
#include "Allocator.hpp"
#include <boost/pool/pool_alloc.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <random>

using wijagels::list;
using wijagels::BasicAllocator;

static std::array<int, 9> push{{1, 2, 3, 4, 5, 6, 7, 8, 9}};

std::initializer_list<int> rand_list{
    1471, 6313, 9678, 9176, 6683, 420,  6651, 9767, 1027, 1854, 9327, 205,
    9076, 356,  4511, 3581, 7617, 456,  5413, 669,  6762, 9944, 1464, 3731,
    2945, 7505, 4286, 690,  3707, 8977, 6927, 9602, 7354, 4482, 7955, 498,
    9460, 7744, 428,  4808, 6265, 9322, 7201, 5507, 2387, 1843, 2523, 6690,
    8314, 4120, 3100, 6877, 6003, 9585, 6979, 1873, 8335, 4765, 1416, 2331,
    7232, 6122, 5815, 7168, 3559, 8512, 1708, 8587, 2273, 797,  4628, 830,
    5621, 1007, 9735, 8854, 9214, 7397, 8169, 2943, 8108, 4143, 7274, 2422,
    5226, 5020, 7930, 8817, 7087, 8819, 365,  1923, 5724, 4674, 3930, 2684,
    7541, 290,  9230, 6477, 4335, 1705, 4817, 2033, 8839, 3893, 2052, 4672,
    2259, 6531, 195,  7829, 994,  6155, 3294, 365,  9602, 4706, 7685, 3565,
    8041, 3679, 7519, 5948, 8631, 7335, 8295, 8122, 802,  6200, 2848, 1553,
    9998, 9829, 3729, 6208, 3721, 8257, 8778, 1448, 2762, 3299, 1628, 3372,
    5400, 6951, 8457, 2355, 9752, 5639, 475,  8182, 2968, 9580, 9261, 1157,
    8029, 4205, 3937, 2657, 698,  5793, 9605, 5061, 7177, 5103, 4429, 1322,
    5348, 4250, 1077, 761,  1173, 4718, 5219, 5,    4758, 895,  5496, 4644,
    5877, 680,  5115, 6234, 9531, 9928, 4295, 3949, 5288, 3273, 6884, 5794,
    3188, 41,   6633, 4044, 3389, 926,  5611, 4146, 5350, 1101, 1878, 2865,
    3496, 1368, 2393, 2833, 9068, 280,  1053, 1924, 2303, 6612, 8552, 5840,
    3754, 7486, 2047, 1206, 7699, 1845, 3061, 1033, 3888, 6218, 3390, 3668,
    2894, 5744, 3875, 6292, 5623, 5971, 2370, 6994, 5695, 6230, 439,  469,
    5736, 3178, 3457, 7978, 3354, 4156, 5265, 2982, 8838, 2454, 5072, 7050,
    8144, 8230, 7308, 7546, 3443, 8505, 8671, 5439, 8013, 2181, 5115, 8689,
    4470, 5538, 1715, 6767, 9585, 8159, 3748, 9047, 7742, 6514, 7589, 8208,
    1682, 1578, 4890, 1585, 4806, 8041, 6091, 1747, 6421, 914,  6030, 8276,
    644,  115,  1333, 8604, 2345, 3749, 1741, 627,  2558, 3817, 6080, 9483,
    5187, 4177, 3011, 9948, 5838, 9326, 1832, 2629, 8274, 2120, 5881, 223,
    7934, 2595, 5037, 2550, 2253, 1877, 5401, 6118, 6755, 4892, 1904, 9556,
    9254, 4212, 8141, 7900, 7259, 5439, 4646, 546,  9091, 47,   1127, 8555,
    6931, 8834, 3913, 6798, 7414, 6541, 6985, 8284, 5810, 7700, 7706, 3379,
    8618, 9432, 8365, 5006, 2155, 5670, 7625, 3532, 3174, 4398, 3931, 9777,
    6212, 1644, 8225, 9192, 4783, 2526, 1119, 2283, 1087, 684,  9747, 1179,
    4012, 6906, 4582, 1173, 8667, 2567, 6323, 7800, 3071, 9709, 4647, 9605,
    8368, 6426, 7022, 4246, 4256, 7023, 8336, 7806, 5116, 9030, 5158, 4217,
    9207, 6179, 7766, 5467, 6231, 802,  6763, 5072, 2973, 2480, 4581, 4877,
    5861, 5473, 961,  3134, 2235, 78,   9819, 8800, 4180, 5421, 1950, 3296,
    6741, 6212, 7600, 300,  9415, 260,  5170, 146,  6932, 9238, 9782, 4143,
    7803, 4145, 4703, 3882, 4935, 215,  8513, 5412, 6920, 7084, 2549, 6082,
    8461, 5788, 3651, 2459, 7789, 5745, 3363, 7426, 777,  5742, 3517, 9442,
    6802, 9879, 294,  5294, 5153, 4204, 8143, 4035, 1308, 8994, 8683, 9239,
    8122, 3238, 4159, 3224, 8541, 7820, 8532, 7022, 9064, 4389, 5183, 3757,
    5458, 1316, 1669, 1654, 4867, 5462, 5634, 7161, 9924, 2933, 9919, 7964,
    4993, 7686, 605,  1852, 7314, 4625, 8951, 1063};

BOOST_AUTO_TEST_CASE(push_back_test) {
  list<int> lst{};
  for (auto e : push) {
    lst.push_back(e);
  }
  auto iterator = lst.begin();
  for (auto e : push) {
    BOOST_REQUIRE(*iterator++ == e);
  }
}

BOOST_AUTO_TEST_CASE(push_front_test) {
  list<int> lst{};
  for (auto e : push) {
    lst.push_front(e);
  }
  auto iterator = --lst.end();
  for (auto e : push) {
    BOOST_REQUIRE(*iterator-- == e);
  }
}

BOOST_AUTO_TEST_CASE(iteration_test) {
  list<int> lst{};
  for (auto e : push) {
    lst.push_front(e);
  }
  BOOST_REQUIRE(std::equal(lst.rbegin(), lst.rend(), push.begin(), push.end()));
}

BOOST_AUTO_TEST_CASE(erase_test) {
  list<int> lst{};
  for (auto e : push) {
    lst.push_back(e);
  }
  BOOST_REQUIRE(std::equal(lst.begin(), lst.end(), push.begin(), push.end()));
  lst.erase(lst.begin());
  BOOST_REQUIRE(
      std::equal(lst.begin(), lst.end(), push.begin() + 1, push.end()));
  while (lst.begin() != lst.end()) lst.erase(lst.begin());
}

BOOST_AUTO_TEST_CASE(emplace_test) {
  list<int> lst{};
  for (auto e : push) {
    lst.emplace_back(e);
  }
  auto iterator = lst.begin();
  for (auto e : push) {
    BOOST_REQUIRE(*iterator++ == e);
  }
}

struct StrictStruct {
  /*
   * This is a struct that cannot be copy, default, or move constructed.
   */
  StrictStruct() = delete;
  StrictStruct(const StrictStruct &) = delete;
  StrictStruct(StrictStruct &&) = delete;
  StrictStruct(int a, double b) : a_{a}, b_{b} {}
  ~StrictStruct() = default;
  StrictStruct &operator=(const StrictStruct &) = delete;
  StrictStruct &operator=(StrictStruct &&) = delete;
  friend bool operator==(const StrictStruct &lhs, const StrictStruct &rhs) {
    return lhs.a_ == rhs.a_ && lhs.b_ == rhs.b_;
  }
  friend bool operator<(const StrictStruct &lhs, const StrictStruct &rhs) {
    return lhs.a_ < rhs.a_;
  }
  int a_;
  double b_;
};

BOOST_AUTO_TEST_CASE(emplace_class_test) {
  list<StrictStruct> lst{};
  std::list<StrictStruct> slst{};
  lst.emplace_back(2, 3.0);
  slst.emplace_back(2, 3.0);

  lst.emplace_front(0, 2.0);
  slst.emplace_front(0, 2.0);
  BOOST_REQUIRE(std::equal(lst.begin(), lst.end(), slst.begin(), slst.end()));
}

BOOST_AUTO_TEST_CASE(resize_test) {
  list<int> lst{};
  lst.resize(1000, 1);
  BOOST_REQUIRE(lst.size() == 1000);
  for (auto e : lst) {
    BOOST_REQUIRE(e == 1);
  }
  lst.resize(10);
  BOOST_REQUIRE(lst.size() == 10);
  for (auto e : lst) {
    BOOST_REQUIRE(e == 1);
  }
}

BOOST_AUTO_TEST_CASE(merge_test) {
  list<int> result{1, 2, 3, 4, 5, 6};
  {
    list<int> lst1{1, 3, 5};
    list<int> lst2{2, 4, 6};
    lst1.merge(lst2);
    BOOST_REQUIRE(
        std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
    BOOST_REQUIRE_EQUAL(lst2.size(), 0);
  }
  {
    list<int> lst1{1, 3, 5};
    list<int> lst2{2, 4, 6};
    lst2.merge(lst1);
    BOOST_REQUIRE(
        std::equal(lst2.begin(), lst2.end(), result.begin(), result.end()));
  }
  {
    list<int> lst1{};
    list<int> lst2 = result;
    lst2.merge(lst1);
    BOOST_REQUIRE(
        std::equal(lst2.begin(), lst2.end(), result.begin(), result.end()));
    lst1.merge(lst2);
    BOOST_REQUIRE(
        std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
  }
  {
    list<int> lst1{1, 2, 6};
    list<int> lst2{3, 4, 5};
    list<int> lst3 = lst1;
    list<int> lst4 = lst2;
    lst2.merge(lst1);
    BOOST_REQUIRE(
        std::equal(lst2.begin(), lst2.end(), result.begin(), result.end()));
    lst3.merge(lst4);
    BOOST_REQUIRE(
        std::equal(lst3.begin(), lst3.end(), result.begin(), result.end()));
  }
  {
    list<int> lst1{1, 2, 3};
    list<int> lst2{4, 5, 6};
    list<int> lst3 = lst1;
    list<int> lst4 = lst2;
    lst1.merge(lst2);
    lst4.merge(lst3);
    BOOST_REQUIRE(
        std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
    BOOST_REQUIRE(
        std::equal(lst4.begin(), lst4.end(), result.begin(), result.end()));
  }
  {
    /* Verify that we aren't copying anything */
    list<StrictStruct> res;
    res.emplace_back(2, 16.0);
    res.emplace_back(2, 3.0);
    res.emplace_back(3, 4.0);
    res.emplace_back(8, 1.0);

    list<StrictStruct> lst1;
    list<StrictStruct> lst2;
    lst1.emplace_back(2, 3.0);
    lst1.emplace_back(3, 4.0);
    lst2.emplace_back(2, 16.0);
    lst2.emplace_back(8, 1.0);
    lst2.merge(lst1);
    BOOST_REQUIRE(std::equal(lst2.begin(), lst2.end(), res.begin(), res.end()));
  }
}

BOOST_AUTO_TEST_CASE(splice_test) {
  list<int> lst1{1, 2, 6};
  list<int> lst2{3, 4, 5};
  list<int> result{1, 2, 6, 3, 4, 5};
  lst1.splice(lst1.cend(), lst2);
  BOOST_REQUIRE(
      std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
}

BOOST_AUTO_TEST_CASE(remove_test) {
  list<int> lst1{0, 1, 1, 1, 30, 0, 0, 1, 0};
  list<int> result{1, 1, 1, 30, 1};
  lst1.remove(0);
  BOOST_REQUIRE(
      std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
}

BOOST_AUTO_TEST_CASE(remove_if_test) {
  list<int> lst1{0, 1, 1, 1, 30, 0, 0, 1, 0};
  list<int> result{1, 1, 1, 30, 1};
  size_t n = lst1.size();
  lst1.remove_if([&n](const int &i) {
    --n;
    return i == 0;
  });
  // Should be exactly size() applications of predicate
  BOOST_REQUIRE_EQUAL(n, 0);
  BOOST_REQUIRE(
      std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
  lst1.remove_if([](const int &) { return true; });
  BOOST_REQUIRE(lst1.empty());
}

BOOST_AUTO_TEST_CASE(reverse_test) {
  list<int> lst1{1, 2, 3};
  list<int> result{3, 2, 1};
  lst1.reverse();
  BOOST_REQUIRE(
      std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
}

BOOST_AUTO_TEST_CASE(unique_test) {
  list<int> lst1{1, 1, 2, 2, 3, 4, 4, 5, 6, 6};
  list<int> result{1, 2, 3, 4, 5, 6};
  lst1.unique();
  BOOST_REQUIRE(
      std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
}

BOOST_AUTO_TEST_CASE(sort_test) {
  list<int> lst1{6, 5, 4, 3, 2, 1};
  list<int> result{1, 2, 3, 4, 5, 6};
  lst1.sort();
  BOOST_REQUIRE(
      std::equal(lst1.begin(), lst1.end(), result.begin(), result.end()));
  list<int> lst2{rand_list};
  std::vector<int> result2{rand_list};
  lst2.sort();
  std::sort(result2.begin(), result2.end());
  BOOST_REQUIRE(
      std::equal(lst2.begin(), lst2.end(), result2.begin(), result2.end()));
}

BOOST_AUTO_TEST_CASE(construct_test) {
  list<StrictStruct> lst1{};
  lst1.emplace_back(1, 2.0);
  lst1.emplace_back(2, 3.0);
  list<StrictStruct> lst2 = std::move(lst1);
  list<int> lst3{list<int>{1, 2, 3}};
  BOOST_REQUIRE(lst3 == (list<int>{1, 2, 3}));
  list<int> lst4{};
  lst4 = std::move(lst3);
  BOOST_REQUIRE(lst4 == (list<int>{1, 2, 3}));
}

BOOST_AUTO_TEST_CASE(large_sort) {
  list<int64_t> lst{};
  for (size_t i = 0; i < 1e5; i++) {
    int64_t n = rand();
    if (n % 2)
      lst.push_back(n);
    else
      lst.push_front(n);
  }
  // BOOST_REQUIRE(lst.verify_integrity());
  lst.sort();
  // BOOST_REQUIRE(lst.verify_integrity());
}

BOOST_AUTO_TEST_CASE(assign_test) {
  auto ilist = {1, 2, 3};
  auto ilist2 = {1, 2, 3, 4};
  list<int> lst1{1, 2, 3};
  list<int> lst2{3, 2, 1};
  lst2 = std::move(lst1);  // Move-assign
  BOOST_REQUIRE(
      std::equal(lst2.begin(), lst2.end(), ilist.begin(), ilist.end()));
  list<int> lst3{3};
  lst3 = lst2;  // Copy-assign
  BOOST_REQUIRE(
      std::equal(lst3.begin(), lst3.end(), ilist.begin(), ilist.end()));
  lst3.push_back(4);
  // Check original is same
  BOOST_REQUIRE(
      std::equal(lst2.begin(), lst2.end(), ilist.begin(), ilist.end()));
  BOOST_REQUIRE(
      std::equal(lst3.begin(), lst3.end(), ilist2.begin(), ilist2.end()));
}

BOOST_AUTO_TEST_CASE(iterator_test) {
  std::list<int> result{1, 2, 8, 16};
  list<int> lst{1, 2, 8, 16};
  BOOST_REQUIRE(
      std::equal(lst.begin(), lst.end(), result.begin(), result.end()));
  BOOST_REQUIRE(
      std::equal(lst.cbegin(), lst.cend(), result.cbegin(), result.cend()));
  BOOST_REQUIRE(
      std::equal(lst.rbegin(), lst.rend(), result.rbegin(), result.rend()));
  BOOST_REQUIRE(
      std::equal(lst.crbegin(), lst.crend(), result.crbegin(), result.crend()));
}

template <typename T>
struct pointer_traits {
  using reference = T &;
  using const_reference = const T &;
};

template <>
struct pointer_traits<void> {};

template <typename T = void>
struct MyAllocator : public pointer_traits<T> {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using pointer = T *;
  using const_pointer = const T *;
  using difference_type =
      typename std::pointer_traits<pointer>::difference_type;

  MyAllocator() = default;

  template <typename U>
  MyAllocator(const MyAllocator<U> &) noexcept {}

  T *allocate(size_t, const void * = nullptr) {
    auto ptr = new T;
    ++counter;
    return ptr;
  }

  void deallocate(T *ptr, size_t) { delete ptr; }

  template <typename U>
  struct rebind {
    using other = MyAllocator<U>;
  };

  size_t counter{};
};
template <typename T, typename U>
constexpr bool operator==(const MyAllocator<T> &,
                          const MyAllocator<U> &) noexcept;

template <typename T, typename U>
constexpr bool operator!=(const MyAllocator<T> &,
                          const MyAllocator<U> &) noexcept;

BOOST_AUTO_TEST_CASE(allocator_test) {
  MyAllocator<int> pool{};
  list<int, decltype(pool)> lst{pool};
  for (auto e : push) {
    lst.emplace_back(e);
  }
  BOOST_REQUIRE(std::equal(lst.begin(), lst.end(), push.begin(), push.end()));
  BOOST_REQUIRE(lst.d_node_alloc.counter == 9);
}
