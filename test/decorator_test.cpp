#include "Decorators.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include <iostream>

void sleeper(long);

void sleeper(long k) {
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
  TimerHook<decltype(sleeper(long{})), long> sleeper_hook;
  make_decorator(sleeper_hook, sleeper)(1l);
  auto[x, y] = make_decorator(add_hook, add<int, int>)(1, 3);
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
  auto f1 = []() {};
  auto f2 = []() {};
  StatelessHook<decltype(f1), decltype(f2), void> hook{f1, f2};
  auto decorated =
      make_decorator<decltype(hook), decltype(wrapme), void>(hook, wrapme);
  auto[decret, ret] = decorated();
  auto x = std::is_same_v<decltype(decret), None>;
  auto y = std::is_same_v<decltype(ret), None>;
  EXPECT_TRUE(x);
  EXPECT_TRUE(y);
}

std::function<std::tuple<None, int>(int)> decorated_fib;

int fib(int n) {
  if (n < 2) return 1;
  return std::get<1>(decorated_fib(n - 2)) + std::get<1>(decorated_fib(n - 1));
}

TEST(decorator_test, memoizer) {  // NOLINT
  {
    auto expensive_fn = [](long i) {
      sleeper(i * 42);
      return 12;
    };
    MemoizerHook<int, long> mem_hook;
    auto decorated =
        make_decorator<decltype(mem_hook), decltype(expensive_fn), int, long>(
            mem_hook, expensive_fn);
    for (long i = 0; i < 4; i++) {
      for (int j = 0; j < 42; j++) {
        decorated(i);
      }
    }
  }
  {
    decorated_fib = make_memoized(fib);
    decorated_fib(58);
  }
  {
    auto fn = [] (int i, int j) { return i*i*i*j; };
    auto decorated = make_memoized(fn);
    auto x = decorated(2, 3).second;
    EXPECT_EQ(x, 24);
  }
}
