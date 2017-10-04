#include "SkipList.hpp"
#include "gtest/gtest.h"
#include <set>

using wijagels::skiplist;

const std::initializer_list<int> g_seed{1, 2, 4, 3};
const std::initializer_list<int> g_sorted{1, 2, 3, 4};
const std::initializer_list<int> g_rand_list{
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

const std::initializer_list<int> g_md_list{
    1471, 6313, 9678, 9176, 6683, 420, 6651, 9767, 1027, 1854, 9327, 205,
    9076, 356,  4511, 420,  7617, 456, 5413, 669,  6762, 9944, 1464, 3731,
};

TEST(skiplist_test, insert_test) {  // NOLINT
  skiplist<int> s{};
  for (auto e : g_seed) {
    s.insert(e);
  }
  EXPECT_TRUE(std::equal(s.begin(), s.end(), g_sorted.begin(), g_sorted.end()));
}

struct nocopy {
  nocopy(int n) : data{n} {}
  nocopy(const nocopy &) = delete;
  nocopy(nocopy &&) = default;
  ~nocopy() = default;
  nocopy &operator=(const nocopy &) = delete;
  nocopy &operator=(nocopy &&) = delete;
  friend bool operator==(const nocopy &lhs, const nocopy &rhs) {
    return lhs.data == rhs.data;
  }
  friend bool operator<(const nocopy &lhs, const nocopy &rhs) {
    return lhs.data < rhs.data;
  }
  int data;
};

TEST(skiplist_test, move_insert_test) {  // NOLINT
  skiplist<nocopy> s{};
  s.insert(nocopy{1});
  s.insert(nocopy{2});
  s.insert(nocopy{3});
  std::initializer_list<nocopy> result = {{1}, {2}, {3}};
  EXPECT_TRUE(std::equal(s.begin(), s.end(), result.begin(), result.end()));
}

TEST(skiplist_test, med_insert_test) {  // NOLINT
  std::set<int> rand_list_sorted = g_md_list;
  skiplist<int> s{};
  for (auto e : g_md_list) {
    s.insert(e);
  }
  EXPECT_TRUE(std::equal(s.begin(), s.end(), rand_list_sorted.begin(),
                         rand_list_sorted.end()));
}

TEST(skiplist_test, big_insert_test) {  // NOLINT
  std::set<int> rand_list_sorted = g_rand_list;
  skiplist<int> s{};
  for (auto e : g_rand_list) {
    s.insert(e);
  }
  EXPECT_TRUE(std::equal(s.begin(), s.end(), rand_list_sorted.begin(),
                         rand_list_sorted.end()));
}

TEST(skiplist_test, big_emplace_test) {  // NOLINT
  std::set<int> rand_list_sorted = g_rand_list;
  skiplist<int> s{};
  for (auto e : g_rand_list) {
    s.emplace(e);
  }
  EXPECT_TRUE(std::equal(s.begin(), s.end(), rand_list_sorted.begin(),
                         rand_list_sorted.end()));
}

TEST(skiplist_test, erase_test) {  // NOLINT
  std::set<int> nums{1, 2, 3, 4, 5};
  skiplist<int> s{};
  for (auto e : nums) {
    s.insert(e);
  }
  s.erase(s.begin());
  nums.erase(nums.begin());
  EXPECT_TRUE(std::equal(s.begin(), s.end(), nums.begin(), nums.end()));
}

TEST(skiplist_test, extract_test) {  // NOLINT
  std::set<int> nums{1, 2, 3, 4, 5};
  auto s = new skiplist<int>{};
  for (auto e : nums) {
    s->insert(e);
  }
  auto nh = s->extract(s->begin());
  s->insert(std::move(nh));
  EXPECT_EQ(1, *s->begin());
}

TEST(skiplist_test, find_test) {  // NOLINT
  std::set<int> nums{1, 2, 3, 4, 5};
  skiplist<int> s{};
  for (auto e : nums) {
    s.insert(e);
  }
  s.insert(12);
  for (auto e : nums) {
    EXPECT_TRUE(*s.find(e) == e);
  }
  s.erase(s.find(12));
  EXPECT_TRUE(s.find(12) == s.end());  // Not in list
}

TEST(skiplist_test, huge_test) {  // NOLINT
  std::uniform_int_distribution<int64_t> distrib{1, 1000};
  std::random_device rd;
  std::mt19937_64 gen{rd()};
  skiplist<int64_t> list;
  for (size_t i = 0; i < 1e6; i++) {
    list.insert(distrib(gen));
  }
  for (size_t i = 0; i < 1e4; i++) {
    auto r = list.find(distrib(gen));
    if (r != list.end()) list.erase(r);
  }
  EXPECT_TRUE(std::is_sorted(list.begin(), list.end()));
}

TEST(skiplist_test, merge_test) {  // NOLINT
  std::initializer_list<int> result1{1, 2, 3, 4, 5, 6, 7};
  std::initializer_list<int> result2{1, 2};
  skiplist<int> list1{1, 2, 3, 5};
  skiplist<int> list2{1, 2, 4, 6, 7};
  list1.merge(list2);
  EXPECT_TRUE(std::is_sorted(list1.begin(), list1.end()));
  EXPECT_TRUE(std::is_sorted(list2.begin(), list2.end()));
  EXPECT_TRUE(
      std::equal(list1.begin(), list1.end(), result1.begin(), result1.end()));
  EXPECT_TRUE(
      std::equal(list2.begin(), list2.end(), result2.begin(), result2.end()));
  list1.verify_integrity();
  list2.verify_integrity();
}

TEST(skiplist_test, iterator_test) {  // NOLINT
  std::set<int> result{3, 5, 1, 7, 16};
  skiplist<int> list{3, 5, 1, 7, 16};
  EXPECT_TRUE(std::is_sorted(list.begin(), list.end()));
  EXPECT_TRUE(
      std::equal(list.begin(), list.end(), result.begin(), result.end()));
  EXPECT_TRUE(
      std::equal(list.cbegin(), list.cend(), result.begin(), result.end()));
  EXPECT_TRUE(
      std::equal(list.rbegin(), list.rend(), result.rbegin(), result.rend()));
  EXPECT_TRUE(std::equal(list.crbegin(), list.crend(), result.crbegin(),
                         result.crend()));
}

TEST(skiplist_test, size_test) {  // NOLINT
  skiplist<int> list{1, 2, 3};
  EXPECT_EQ(list.size(), 3);
  list.insert(4);
  EXPECT_EQ(list.size(), 4);
  list.erase(list.find(4));
  EXPECT_EQ(list.size(), 3);
  list.clear();
  EXPECT_EQ(list.size(), 0);
}

TEST(skiplist_test, move_test) {  // NOLINT
  skiplist<int> list{g_rand_list};
  skiplist<int> nls{1};
  for (int i = 0; i < 1e4; i++) {
    nls = std::move(list);
    list = std::move(nls);
  }
  skiplist<int> result{g_rand_list};
  EXPECT_TRUE(
      std::equal(list.begin(), list.end(), result.begin(), result.end()));
}
