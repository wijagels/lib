#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE vector test
#include "Vector.hpp"
#include <boost/test/unit_test.hpp>
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

BOOST_AUTO_TEST_CASE(basic_test) {
  vector<int> v;
  v.push_back(1);
}

BOOST_AUTO_TEST_CASE(obj_test) {
  vector<std::pair<int, int>> v;
  v.push_back({1, 2});
  std::pair<int, int> p = {8, 5};
  v.push_back(p);
  BOOST_REQUIRE_EQUAL(v.back().first, p.first);
  BOOST_REQUIRE_EQUAL(v.back().second, p.second);
  vector<MoveStruct> mv;
  mv.emplace_back(1, 2.0);
  MoveStruct s{1, 2.0};
  BOOST_REQUIRE(s == mv.back());
  mv.emplace_back(2, 2.0);
  mv.emplace_back(3, 2.0);
  mv.emplace_back(4, 2.0);
  mv.emplace_back(5, 2.0);
}

BOOST_AUTO_TEST_CASE(copy_only_test) {
  {
    vector<SlowStruct> mv;
    for (int i = 0; i < 65; i++) {
      mv.emplace_back();
    }
    BOOST_REQUIRE_EQUAL(SlowStruct::instance_count, 65);
  }
  BOOST_REQUIRE_EQUAL(SlowStruct::instance_count, 0);
}

BOOST_AUTO_TEST_CASE(initializer_list_test) {
  std::initializer_list<int> init = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  BOOST_REQUIRE_EQUAL(v.size(), 10);
  BOOST_REQUIRE_EQUAL_COLLECTIONS(v.begin(), v.end(), init.begin(), init.end());
}

BOOST_AUTO_TEST_CASE(equality_test) {
  vector<int> v1, v2;
  v1.push_back(2);
  v1.push_back(3);
  v2.push_back(2);
  v2.push_back(3);
  BOOST_REQUIRE(v1 == v2);
  v1.clear();
  BOOST_REQUIRE(v1 != v2);
}

BOOST_AUTO_TEST_CASE(index_test) {
  vector<size_t> v1{0, 1, 2, 3, 4, 5};
  for (size_t i = 0; i < 6; i++) {
    BOOST_REQUIRE_EQUAL(i, v1[i]);
  }
  vector<int> v2;
  v2.push_back(6);
  v2.push_back(7);
  BOOST_REQUIRE_EQUAL(v2.size(), 2);
  BOOST_REQUIRE_EQUAL(6, v2[0]);
  BOOST_REQUIRE_EQUAL(7, v2[1]);
}

BOOST_AUTO_TEST_CASE(const_test) {
  const vector<std::string> v1{{"a", "b", "c"}};
  auto it = v1.begin();
  ++it;
}

BOOST_AUTO_TEST_CASE(move_test) {
  {
    vector<std::string> v1{{"a", "b", "c"}};
    vector<std::string> v3{v1};
    vector<std::string> v2{std::move(v1)};
    BOOST_REQUIRE_EQUAL_COLLECTIONS(v2.begin(), v2.end(), v3.begin(), v3.end());
  }
  {
    vector<std::string> v1{{"a", "b", "c"}};
    vector<std::string> v3{"1"};
    v3 = v1;
    vector<std::string> v2{"2"};
    v2 = std::move(v1);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(v2.begin(), v2.end(), v3.begin(), v3.end());
  }
}

BOOST_AUTO_TEST_CASE(iterator_test) {
  using iterator = vector<std::string>::iterator;
  using const_iterator = vector<std::string>::const_iterator;
  vector<std::string> v{{"a", "b", "c"}};
  iterator it = v.begin();
  const_iterator cit = it;
  BOOST_REQUIRE_EQUAL(*cit, *it);
}

BOOST_AUTO_TEST_CASE(erase_test) {
  vector<std::string> v1{{"a", "b", "c"}};
  vector<std::string> v2{{"b"}, {"c"}};
  vector<std::string> v3{{"c"}};
  v1.erase(v1.begin());
  BOOST_REQUIRE_EQUAL_COLLECTIONS(v1.begin(), v1.end(), v2.begin(), v2.end());
  v1.erase(v1.begin());
  BOOST_REQUIRE_EQUAL_COLLECTIONS(v1.begin(), v1.end(), v3.begin(), v3.end());
  v1.erase(v1.begin());
  BOOST_REQUIRE(v1.empty());
  v1.erase(v1.begin(), v1.end());
  BOOST_REQUIRE(v1.empty());
}
