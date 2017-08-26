#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Map test
#include <boost/test/unit_test.hpp>
#include <utility>

#include "List.hpp"
#include "Map.hpp"
using wijagels::list;
using wijagels::map;

BOOST_AUTO_TEST_CASE(construct_test) { map<int, int> m{}; }

BOOST_AUTO_TEST_CASE(at_test) {
  map<int, int> m{};
  m.insert({1, 2});
  BOOST_REQUIRE_EQUAL(2, m.at(1));
}

BOOST_AUTO_TEST_CASE(index_op_test) {
  map<int, int> m{};
  // Call with const&
  int i = 3;
  BOOST_REQUIRE_EQUAL(int{}, m[i]);

  // Call with temporary
  BOOST_REQUIRE_EQUAL(int{}, m[4]);
  m[4]++;
  BOOST_REQUIRE_EQUAL(int{} + 1, m[4]);
}

BOOST_AUTO_TEST_CASE(iteration_test) {
  list<std::pair<const int, int>> result = {{2, 4}, {3, 9}, {4, 18}};
  map<int, int> m{{2, 4}, {3, 9}, {4, 18}};
  BOOST_REQUIRE(std::equal(m.begin(), m.end(), result.begin(), result.end()));
  BOOST_REQUIRE(
      std::equal(m.cbegin(), m.cend(), result.cbegin(), result.cend()));
  BOOST_REQUIRE(
      std::equal(m.rbegin(), m.rend(), result.rbegin(), result.rend()));
  BOOST_REQUIRE(
      std::equal(m.crbegin(), m.crend(), result.crbegin(), result.crend()));
  result.erase(result.begin());
  m.erase(m.find(2));
  BOOST_REQUIRE(std::equal(m.begin(), m.end(), result.begin(), result.end()));
  BOOST_REQUIRE(
      std::equal(m.cbegin(), m.cend(), result.cbegin(), result.cend()));
  BOOST_REQUIRE(
      std::equal(m.rbegin(), m.rend(), result.rbegin(), result.rend()));
  BOOST_REQUIRE(
      std::equal(m.crbegin(), m.crend(), result.crbegin(), result.crend()));
}

BOOST_AUTO_TEST_CASE(extract_test) {
  //  Roundabout way of figuring out the type
  using node_type = decltype(std::declval<map<int, int>>().extract(
      std::declval<map<int, int>>().begin()));
  node_type nh1, nh2;
  {
    map<int, int> m{{2, 4}, {6, 8}};
    nh1 = m.extract(m.begin());
    nh2 = m.extract(m.begin());
  }  // m goes out of scope
  BOOST_REQUIRE_EQUAL(nh1.key(), 2);
  BOOST_REQUIRE_EQUAL(nh1.mapped(), 4);
  BOOST_REQUIRE_EQUAL(nh2.key(), 6);
  BOOST_REQUIRE_EQUAL(nh2.mapped(), 8);
  map<int, int> m{};
  m.insert(std::move(nh1));
  m.insert(m.end(), std::move(nh2));
  BOOST_REQUIRE_EQUAL(m.begin()->first, 2);
  BOOST_REQUIRE_EQUAL(m.begin()->second, 4);
  BOOST_REQUIRE_EQUAL((++m.begin())->first, 6);
  BOOST_REQUIRE_EQUAL((++m.begin())->second, 8);
}

BOOST_AUTO_TEST_CASE(try_emplace_test) {
  map<int, int> m{{2, 4}, {6, 8}};
  m.try_emplace(1, 3);
  BOOST_REQUIRE_EQUAL(m[1], 3);
  auto r = m.try_emplace(1, 5);
  BOOST_REQUIRE_EQUAL(r.second, false);
}

BOOST_AUTO_TEST_CASE(insert_or_assign_test) {
  map<int, int> m{{8, 9}};
  auto r = m.insert_or_assign(1, 2);
  BOOST_REQUIRE_EQUAL(r.second, true);
  BOOST_REQUIRE_EQUAL(r.first->first, 1);
  BOOST_REQUIRE_EQUAL(r.first->second, 2);
  BOOST_REQUIRE_EQUAL(m.find(1)->first, 1);
  BOOST_REQUIRE_EQUAL(m.find(1)->second, 2);
  r = m.insert_or_assign(1, 3);
  BOOST_REQUIRE_EQUAL(r.second, false);
  BOOST_REQUIRE_EQUAL(r.first->first, 1);
  BOOST_REQUIRE_EQUAL(r.first->second, 3);
  BOOST_REQUIRE_EQUAL(m.find(1)->first, 1);
  BOOST_REQUIRE_EQUAL(m.find(1)->second, 3);

  m.insert_or_assign(m.find(1), 1, 4);
  BOOST_REQUIRE_EQUAL(m.find(1)->first, 1);
  BOOST_REQUIRE_EQUAL(m.find(1)->second, 4);
}
