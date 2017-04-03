#include "Decorators.hpp"
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Decorator test
#include <boost/test/unit_test.hpp>
#include <cstdlib>

void sleeper(int);

void sleeper(int k) {
  timespec ts{0, k};
  nanosleep(&ts, NULL);
}

template <typename T, typename U>
int add(T a, U b) {
  return a + b;
}

BOOST_AUTO_TEST_CASE(timer_test) {
  makeDecorator(sleeper)(1);
  auto x = makeDecorator(add<int, int>)(1, 3);
  BOOST_REQUIRE_EQUAL(x, 4);
}
