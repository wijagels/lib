#include "algorithm.hpp"
#include "gtest/gtest.h"
#include <numeric>
#include <random>
#include <vector>

TEST(algorithm_test, count_test) {  // NOLINT
  {
    std::vector<int> v{1, 1, 2, 3, 8, 0, 1};
    EXPECT_EQ(std::count(v, 1), 3);
    EXPECT_EQ(std::count(std::move(v), 1), 3);
  }
  {
    const std::vector<int> v{1, 1, 2, 3, 8, 0, 1};
    EXPECT_EQ(std::count(static_cast<const std::vector<int> &>(v), 1), 3);
    EXPECT_EQ(std::count(v, 1), 3);
  }
}

TEST(algorithm_test, all_of_test) {  // NOLINT
  std::vector<int> v{1, 1, 2, 3, 8, 0, 1};
  EXPECT_TRUE(std::all_of(v, [](int x) { return x < 9; }));
}

TEST(algorithm_test, foreach_test) {  // NOLINT
  std::vector<int> v1{1, 1, 2, 3, 8, 0, 1};
  std::vector<int> v2{2, 2, 3, 4, 9, 1, 2};
  std::for_each(v1, [](int &x) { return ++x; });
  EXPECT_EQ(v1, v2);
}

TEST(algorithm_test, parallel_reduce) {  // NOLINT
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
  std::vector<int> v;
  v.reserve(1<<22);
  std::generate_n(std::back_inserter(v), 1<<22, [&]() { return dis(gen); });
  auto r1 = std::reduce(std::execution::par_unseq, begin(v), end(v), 0,
                        std::plus<>{});
  auto r2 = std::accumulate(begin(v), end(v), 0);
  EXPECT_EQ(r1, r2);
}
