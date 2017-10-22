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
  auto [x, y] = make_decorator(add_hook, add<int, int>)(1, 3);
  auto x_is_none = std::is_same_v<decltype(x), None>;
  EXPECT_TRUE(x_is_none);
  EXPECT_EQ(make_decorator(add_hook, add<int, int>).target()(3, 2), 5);
  EXPECT_EQ(y, 4);
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

TEST(decorator_test, void_function) {  // NOLINT
  auto wrapme = []() {};
  auto f1 = []() { };
  auto f2 = []() { };
  StatelessHook<decltype(f1), decltype(f2), void> hook{f1, f2};
  auto decorated = make_decorator(hook, wrapme);
  auto [decret, ret] = decorated();
  auto x = std::is_same_v<decltype(decret), None>;
  auto y = std::is_same_v<decltype(ret), None>;
  EXPECT_TRUE(x);
  EXPECT_TRUE(y);
}
