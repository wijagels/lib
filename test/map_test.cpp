#include "gtest/gtest.h"
#include <utility>

#include "List.hpp"
#include "Map.hpp"
using wijagels::list;
using wijagels::map;

TEST(map_test, construct_test) {  // NOLINT
  map<int, int> m{};
}

TEST(map_test, at_test) {  // NOLINT
  map<int, int> m{};
  ASSERT_THROW(m.at(1), std::out_of_range);
  m.insert({1, 2});
  EXPECT_EQ(2, m.at(1));
}

TEST(map_test, index_test) {  // NOLINT
  map<int, int> m;
  // Call with const&
  int i = 3;
  EXPECT_EQ(int{}, m[i]);

  // Call with temporary
  EXPECT_EQ(int{}, m[4]);
  ++m[4];
  EXPECT_EQ(int{} + 1, m[4]);
}

TEST(map_test, clear_test) {  // NOLINT
  map<int, int> m{{1, 2}};
  EXPECT_TRUE(!m.empty());
  m.clear();
  EXPECT_TRUE(m.empty());
}

TEST(map_test, iteration_test) {  // NOLINT
  list<std::pair<const int, int>> result = {{2, 4}, {3, 9}, {4, 18}};
  map<int, int> m{{2, 4}, {3, 9}, {4, 18}};
  EXPECT_TRUE(std::equal(m.begin(), m.end(), result.begin(), result.end()));
  EXPECT_TRUE(std::equal(m.cbegin(), m.cend(), result.cbegin(), result.cend()));
  EXPECT_TRUE(std::equal(m.rbegin(), m.rend(), result.rbegin(), result.rend()));
  EXPECT_TRUE(
      std::equal(m.crbegin(), m.crend(), result.crbegin(), result.crend()));
  result.erase(result.begin());
  m.erase(m.find(2));
  EXPECT_TRUE(std::equal(m.begin(), m.end(), result.begin(), result.end()));
  EXPECT_TRUE(std::equal(m.cbegin(), m.cend(), result.cbegin(), result.cend()));
  EXPECT_TRUE(std::equal(m.rbegin(), m.rend(), result.rbegin(), result.rend()));
  EXPECT_TRUE(
      std::equal(m.crbegin(), m.crend(), result.crbegin(), result.crend()));
}

TEST(map_test, extract_test) {  // NOLINT
  //  Roundabout way of figuring out the type
  using node_type = decltype(std::declval<map<int, int>>().extract(
      std::declval<map<int, int>>().begin()));
  node_type nh1, nh2;
  {
    map<int, int> m{{2, 4}, {6, 8}};
    nh1 = m.extract(m.begin());
    nh2 = m.extract(m.begin());
  }  // m goes out of scope
  EXPECT_EQ(nh1.key(), 2);
  EXPECT_EQ(nh1.mapped(), 4);
  EXPECT_EQ(nh2.key(), 6);
  EXPECT_EQ(nh2.mapped(), 8);
  map<int, int> m{};
  m.insert(std::move(nh1));
  m.insert(m.end(), std::move(nh2));
  EXPECT_EQ(m.begin()->first, 2);
  EXPECT_EQ(m.begin()->second, 4);
  EXPECT_EQ((++m.begin())->first, 6);
  EXPECT_EQ((++m.begin())->second, 8);
}

TEST(map_test, try_emplace_test) {  // NOLINT
  map<int, int> m{{2, 4}, {6, 8}};
  m.try_emplace(1, 3);
  EXPECT_EQ(m[1], 3);
  auto r = m.try_emplace(1, 5);
  EXPECT_EQ(r.second, false);
}

TEST(map_test, insert_or_assign_test) {  // NOLINT
  map<int, int> m{{8, 9}};
  auto r = m.insert_or_assign(1, 2);
  EXPECT_EQ(r.second, true);
  EXPECT_EQ(r.first->first, 1);
  EXPECT_EQ(r.first->second, 2);
  EXPECT_EQ(m.find(1)->first, 1);
  EXPECT_EQ(m.find(1)->second, 2);
  r = m.insert_or_assign(1, 3);
  EXPECT_EQ(r.second, false);
  EXPECT_EQ(r.first->first, 1);
  EXPECT_EQ(r.first->second, 3);
  EXPECT_EQ(m.find(1)->first, 1);
  EXPECT_EQ(m.find(1)->second, 3);

  m.insert_or_assign(m.find(1), 1, 4);
  EXPECT_EQ(m.find(1)->first, 1);
  EXPECT_EQ(m.find(1)->second, 4);
}
