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
