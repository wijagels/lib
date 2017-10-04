#include "algorithm.hpp"
#include "gtest/gtest.h"
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
