#include "Decorators.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include <iostream>

void sleeper(int);

void sleeper(int k) {
  std::cout << "sleeping" << std::endl;
  timespec ts{0, k};
  nanosleep(&ts, nullptr);
}

template <typename T, typename U>
auto add(T a, U b) {
  return a + b;
}

struct AddFunctor {
  int operator()(int a, int b) { return a + b; }
};

TEST(decorator_test, timer_test) {  // NOLINT
  TimerHook<decltype(add(int{}, int{})), int, int> add_hook;
  TimerHook<decltype(sleeper(int{})), int> sleeper_hook;
  make_decorator(sleeper_hook, sleeper)(1);
  auto x = make_decorator(add_hook, add<int, int>)(1, 3);
  EXPECT_EQ(make_decorator(add_hook, add<int, int>).target()(3, 2), 5);
  EXPECT_EQ(x, 4);
}

TEST(decorator_test, functor) {  // NOLINT
  TimerHook<std::invoke_result_t<AddFunctor, int, int>, int, int> add_hook;
  auto decorated =
      make_decorator<decltype(add_hook), AddFunctor, int, int, int>(
          add_hook, AddFunctor{});
  decorated(5, 6);
}

TEST(decorator_test, stateless) {  // NOLINT
  auto f1 = [](int i, int j) { std::cout << "pre " << i << " " << j << '\n'; };
  auto f2 = [](int i) {
    std::cout << "post " << i << '\n';
    return 42;
  };
  StatelessHook<decltype(f1), decltype(f2), int, int, int> hook{f1, f2};
  auto decorated = make_decorator(hook, add<int, int>);
  auto[decret, ret] = decorated(1, 2);
  std::cout << decret << " " << ret << '\n';
}
