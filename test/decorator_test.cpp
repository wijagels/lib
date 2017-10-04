#include "Decorators.hpp"
#include "gtest/gtest.h"
#include <cstdlib>

void sleeper(int);

void sleeper(int k) {
  timespec ts{0, k};
  nanosleep(&ts, nullptr);
}

template <typename T, typename U>
int add(T a, U b) {
  return a + b;
}

TEST(decorator_test, timer_test) {
  makeDecorator(sleeper)(1);
  auto x = makeDecorator(add<int, int>)(1, 3);
  EXPECT_EQ(x, 4);
}
