#include "Vector.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <utility>

using wijagels::vector;

struct MoveStruct {
  /*
   * This is a struct that cannot be copy or default constructed.
   * It also cannot be copy or move assigned.
   */
  MoveStruct() = delete;
  MoveStruct(const MoveStruct &) = delete;
  MoveStruct(MoveStruct &&x) : a_{std::move(x.a_)}, b_{std::move(x.b_)} {}
  MoveStruct(int a, double b) : a_{a}, b_{b} {}
  ~MoveStruct() = default;
  MoveStruct &operator=(const MoveStruct &) = delete;
  MoveStruct &operator=(MoveStruct &&) = delete;
  friend bool operator==(const MoveStruct &lhs, const MoveStruct &rhs) {
    return lhs.a_ == rhs.a_ && lhs.b_ == rhs.b_;
  }
  friend bool operator<(const MoveStruct &lhs, const MoveStruct &rhs) {
    return lhs.a_ < rhs.a_;
  }
  int a_;
  double b_;
};

struct SlowStruct {
  /*
   * Dummy struct that has move semantics deleted
   */
  SlowStruct() { ++instance_count; }
  ~SlowStruct() { --instance_count; }
  SlowStruct(const SlowStruct &) { ++SlowStruct::instance_count; }
  SlowStruct(SlowStruct &&) = delete;
  SlowStruct &operator=(const SlowStruct &x) {
    if (&x == this) return *this;
    ++instance_count;
    return *this;
  }
  SlowStruct &operator=(SlowStruct &&) = delete;
  static int instance_count;
};
int SlowStruct::instance_count = 0;

TEST(vector_test, basic_test) {  // NOLINT
  vector<int> v;
  v.push_back(1);
}

TEST(vector_test, obj_test) {  // NOLINT
  vector<std::pair<int, int>> v;
  v.push_back({1, 2});
  std::pair<int, int> p = {8, 5};
  v.push_back(p);
  EXPECT_EQ(v.back().first, p.first);
  EXPECT_EQ(v.back().second, p.second);
  vector<MoveStruct> mv;
  mv.emplace_back(1, 2.0);
  MoveStruct s{1, 2.0};
  EXPECT_TRUE(s == mv.back());
  mv.emplace_back(2, 2.0);
  mv.emplace_back(3, 2.0);
  mv.emplace_back(4, 2.0);
  mv.emplace_back(5, 2.0);
}

TEST(vector_test, copy_only_test) {  // NOLINT
  {
    vector<SlowStruct> mv;
    for (int i = 0; i < 65; i++) {
      mv.emplace_back();
    }
    EXPECT_EQ(SlowStruct::instance_count, 65);
  }
  EXPECT_EQ(SlowStruct::instance_count, 0);
}

TEST(vector_test, initializer_list_test) {  // NOLINT
  std::initializer_list<int> init = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  EXPECT_EQ(v.size(), 10);
  EXPECT_EQ(v, init);
}

TEST(vector_test, equality_test) {  // NOLINT
  vector<int> v1, v2;
  v1.push_back(2);
  v1.push_back(3);
  v2.push_back(2);
  v2.push_back(3);
  EXPECT_TRUE(v1 == v2);
  v1.clear();
  EXPECT_TRUE(v1 != v2);
}

TEST(vector_test, index_test) {  // NOLINT
  vector<size_t> v1{0, 1, 2, 3, 4, 5};
  for (size_t i = 0; i < 6; i++) {
    EXPECT_EQ(i, v1[i]);
  }
  vector<int> v2;
  v2.push_back(6);
  v2.push_back(7);
  EXPECT_EQ(v2.size(), 2);
  EXPECT_EQ(6, v2[0]);
  EXPECT_EQ(7, v2[1]);
}

TEST(vector_test, const_test) {  // NOLINT
  const vector<std::string> v1{{"a", "b", "c"}};
  auto it = v1.begin();
  ++it;
}

TEST(vector_test, move_test) {  // NOLINT
  {
    vector<std::string> v1{{"a", "b", "c"}};
    vector<std::string> v3{v1};
    vector<std::string> v2{std::move(v1)};
    EXPECT_EQ(v2, v3);
  }
  {
    vector<std::string> v1{{"a", "b", "c"}};
    vector<std::string> v3{"1"};
    v3 = v1;
    vector<std::string> v2{"2"};
    v2 = std::move(v1);
    EXPECT_EQ(v2, v3);
  }
}

TEST(vector_test, iterator_test) {  // NOLINT
  using iterator = vector<std::string>::iterator;
  using const_iterator = vector<std::string>::const_iterator;
  vector<std::string> v{{"a", "b", "c"}};
  iterator it = v.begin();
  const_iterator cit = it;
  EXPECT_EQ(*cit, *it);
}

TEST(vector_test, erase_test) {  // NOLINT
  vector<std::string> v1{{"a", "b", "c"}};
  vector<std::string> v2{{"b"}, {"c"}};
  vector<std::string> v3{{"c"}};
  v1.erase(v1.begin());
  EXPECT_EQ(v1, v2);
  v1.erase(v1.begin());
  EXPECT_EQ(v1, v3);
  v1.erase(v1.begin());
  EXPECT_TRUE(v1.empty());
  v1.erase(v1.begin(), v1.end());
  EXPECT_TRUE(v1.empty());
}
