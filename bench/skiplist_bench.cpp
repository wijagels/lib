#include "SkipList.hpp"
#include <benchmark/benchmark.h>
#include <boost/pool/pool_alloc.hpp>

// template <typename T>
// using skiplist = typename wijagels::skiplist<T, std::less<T>,
// boost::fast_pool_allocator<T>>;
using wijagels::skiplist;

std::random_device rd;
std::mt19937 gen{rd()};

static void BM_Default_Constructor(benchmark::State &state) {
  for (auto _ : state) {
    skiplist<int> dest;
    benchmark::DoNotOptimize(dest);
  }
}
BENCHMARK(BM_Default_Constructor);

/**
 * Demonstrate the efficiency of the copy constructor due to hinting
 */
static void BM_Copy_Constructor(benchmark::State &state) {
  skiplist<int> src;
  std::uniform_int_distribution<> dis{};
  for (int i = 0; i < state.range(0); i++) {
    src.insert(dis(gen));
  }
  for (auto _ : state) {
    skiplist<int> dest{src};
  }
  state.SetComplexityN(src.size());
}
BENCHMARK(BM_Copy_Constructor)
    ->RangeMultiplier(2)
    ->Range(1 << 7, 1 << 14)
    ->Complexity();

static void BM_Copy_Manually(benchmark::State &state) {
  skiplist<int> src;
  std::uniform_int_distribution<> dis{};
  for (int i = 0; i < state.range(0); i++) {
    src.insert(dis(gen));
  }
  for (auto _ : state) {
    skiplist<int> dest;
    for (const auto &e : src) {
      dest.insert(e);
    }
  }
  state.SetComplexityN(src.size());
}
BENCHMARK(BM_Copy_Manually)
    ->RangeMultiplier(2)
    ->Range(1 << 7, 1 << 14)
    ->Complexity();

static void BM_Insert(benchmark::State &state) {
  std::vector<int> src;
  src.reserve(static_cast<decltype(src)::size_type>(state.range(0)));
  std::uniform_int_distribution<> dis{};
  for (int i = 0; i < state.range(0); i++) {
    src.push_back(dis(gen));
  }
  for (auto _ : state) {
    skiplist<int> dest;
    for (const auto &e : src) {
      dest.insert(e);
    }
  }
  state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_Insert)->RangeMultiplier(2)->Range(1 << 7, 1 << 14)->Complexity();

static void BM_Insert_Set(benchmark::State &state) {
  std::vector<int> src;
  src.reserve(static_cast<decltype(src)::size_type>(state.range(0)));
  std::uniform_int_distribution<> dis{};
  for (int i = 0; i < state.range(0); i++) {
    src.push_back(dis(gen));
  }
  for (auto _ : state) {
    std::set<int> dest;
    for (const auto &e : src) {
      dest.insert(e);
    }
  }
  state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_Insert_Set)
    ->RangeMultiplier(2)
    ->Range(1 << 7, 1 << 14)
    ->Complexity();

static void BM_Find(benchmark::State &state) {
  std::vector<int> src;
  src.reserve(static_cast<decltype(src)::size_type>(state.range(0)));
  std::uniform_int_distribution<> dis{};
  for (int i = 0; i < state.range(0); i++) {
    src.push_back(dis(gen));
  }
  skiplist<int> list{};
  list.insert(src.begin(), src.end());
  for (auto _ : state) {
    for (const auto &e : src) {
      benchmark::DoNotOptimize(list.find(e));
    }
  }
  state.SetComplexityN(list.size());
}
BENCHMARK(BM_Find)->RangeMultiplier(2)->Range(1 << 7, 1 << 14)->Complexity();

static void BM_Erase(benchmark::State &state) {
  std::vector<int> src;
  src.reserve(static_cast<decltype(src)::size_type>(state.range(0)));
  std::uniform_int_distribution<> dis{};
  for (int i = 0; i < state.range(0); i++) {
    src.push_back(dis(gen));
  }
  skiplist<int> list{};
  list.insert(src.begin(), src.end());
  for (auto _ : state) {
    state.PauseTiming();
    skiplist<int> copy{list};
    state.ResumeTiming();
    for (const auto &e : src) {
      copy.erase(e);
    }
  }
  state.SetComplexityN(list.size());
}
BENCHMARK(BM_Erase)->RangeMultiplier(2)->Range(1 << 7, 1 << 14)->Complexity();

static void BM_Erase_Set(benchmark::State &state) {
  std::vector<int> src;
  src.reserve(static_cast<decltype(src)::size_type>(state.range(0)));
  std::uniform_int_distribution<> dis{};
  for (int i = 0; i < state.range(0); i++) {
    src.push_back(dis(gen));
  }
  std::set<int> list{};
  list.insert(src.begin(), src.end());
  for (auto _ : state) {
    state.PauseTiming();
    std::set<int> copy{list};
    state.ResumeTiming();
    for (const auto &e : src) {
      copy.erase(e);
    }
  }
  state.SetComplexityN(list.size());
}
BENCHMARK(BM_Erase_Set)
    ->RangeMultiplier(2)
    ->Range(1 << 7, 1 << 14)
    ->Complexity();

BENCHMARK_MAIN();
