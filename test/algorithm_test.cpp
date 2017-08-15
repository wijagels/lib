#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Map test
#include "algorithm.hpp"
#include <boost/test/unit_test.hpp>
#include <vector>

BOOST_AUTO_TEST_CASE(count_test) {
  {
    std::vector<int> v{1, 1, 2, 3, 8, 0, 1};
    BOOST_REQUIRE_EQUAL(std::count(v, 1), 3);
    BOOST_REQUIRE_EQUAL(std::count(std::move(v), 1), 3);
  }
  {
    const std::vector<int> v{1, 1, 2, 3, 8, 0, 1};
    BOOST_REQUIRE_EQUAL(std::count(static_cast<const std::vector<int> &>(v), 1),
                        3);
    BOOST_REQUIRE_EQUAL(std::count(v, 1), 3);
  }
}

BOOST_AUTO_TEST_CASE(all_of_test) {
  std::vector<int> v{1, 1, 2, 3, 8, 0, 1};
  BOOST_REQUIRE(std::all_of(v, [](int x) { return x < 9; }));
}

BOOST_AUTO_TEST_CASE(foreach_test) {
  std::vector<int> v1{1, 1, 2, 3, 8, 0, 1};
  std::vector<int> v2{2, 2, 3, 4, 9, 1, 2};
  std::for_each(v1, [](int &x) { return ++x; });
  BOOST_REQUIRE_EQUAL_COLLECTIONS(v1.begin(), v1.end(), v2.begin(), v2.end());
}
