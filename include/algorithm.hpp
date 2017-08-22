// Copyright 2017 William Jagels
#ifndef INCLUDE_ALGORITHM_HPP_
#define INCLUDE_ALGORITHM_HPP_
/*
 * A set of extensions to <algorithm>
 */

#include <algorithm>
#include <functional>
#include <utility>
#if __has_include(<execution>)
#include <execution>
#endif

#define ONE_CONTAINER_ONE_FUNCTION(function)             \
  template <typename Container, typename Func>           \
  auto function(Container &&c, Func &&f) {               \
    return function(begin(c), end(c), forward<Func>(f)); \
  }

#if defined(__cpp_lib_parallel_algorithm) && \
    __cpp_lib_parallel_algorithm >= 201603
#define ONE_CONTAINER_ONE_FUNCTION_EXEC_POLICY(function)                    \
  template <typename ExecutionPolicy, typename Container, typename Func,    \
            typename std::enable_if<                                        \
                std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>>> \
  auto function(ExecutionPolicy &&policy, Container &&c, Func &&f) {        \
    return function(forward<ExecutionPolicy>(policy), begin(c), end(c),     \
                    forward<Func>(f));                                      \
  }
#else
#define ONE_CONTAINER_ONE_FUNCTION_EXEC_POLICY(function)
#endif

#define ONE_CONTAINER_ONE_FUNCTION_COMBINED(function) \
  ONE_CONTAINER_ONE_FUNCTION(function)                \
  ONE_CONTAINER_ONE_FUNCTION_EXEC_POLICY(function)

#define ONE_CONTAINER_ONE_VALUE(function)      \
  template <typename Container, typename T>    \
  auto function(Container &&c, const T &val) { \
    return function(begin(c), end(c), val);    \
  }

#if defined(__cpp_lib_parallel_algorithm) && \
    __cpp_lib_parallel_algorithm >= 201603
#define ONE_CONTAINER_ONE_VALUE_EXEC_POLICY(function)                         \
  template <typename ExecutionPolicy, typename Container, typename T,         \
            typename std::enable_if<                                          \
                std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>>>   \
  auto function(ExecutionPolicy &&policy, Container &&c, const T &val) {      \
    return function(forward<ExecutionPolicy>(policy), begin(c), end(c), val); \
  }
#else
#define ONE_CONTAINER_ONE_VALUE_EXEC_POLICY(function)
#endif

#define ONE_CONTAINER_ONE_VALUE_COMBINED(function) \
  ONE_CONTAINER_ONE_VALUE(function)                \
  ONE_CONTAINER_ONE_VALUE_EXEC_POLICY(function)

#define TWO_CONTAINERS(function)                             \
  template <typename Container1, typename Container2>        \
  auto function(Container1 &&c1, Container2 &&c2) {          \
    return function(begin(c1), end(c1), begin(c2), end(c2)); \
  }

#if defined(__cpp_lib_parallel_algorithm) && \
    __cpp_lib_parallel_algorithm >= 201603
#define TWO_CONTAINERS_EXEC_POLICY(function)                                  \
  template <typename ExecutionPolicy, typename Container1,                    \
            typename Container2,                                              \
            typename std::enable_if<                                          \
                std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>>>   \
  auto function(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2) { \
    return function(begin(c1), end(c1), begin(c2), end(c2));                  \
  }
#else
#define TWO_CONTAINERS_EXEC_POLICY(function)
#endif

#define TWO_CONTAINERS_COMBINED(function) \
  TWO_CONTAINERS(function)                \
  TWO_CONTAINERS_EXEC_POLICY(function)

#define TWO_CONTAINERS_ONE_FUNCTION(function)                                  \
  template <typename Container1, typename Container2, typename Func>           \
  auto function(Container1 &&c1, Container2 &&c2, Func &&f) {                  \
    return function(begin(c1), end(c1), begin(c2), end(c2), forward<Func>(f)); \
  }

#if defined(__cpp_lib_parallel_algorithm) && \
    __cpp_lib_parallel_algorithm >= 201603
#define TWO_CONTAINERS_ONE_FUNCTION_EXEC_POLICY(function)                   \
  template <typename ExecutionPolicy, typename Container1,                  \
            typename Container2, typename Func,                             \
            typename std::enable_if<                                        \
                std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>>> \
  auto function(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2, \
                Func &&f) {                                                 \
    return function(forward<ExecutionPolicy>(policy), begin(c1), end(c1),   \
                    begin(c2), end(c2), forward<Func>(f));                  \
  }
#else
#define TWO_CONTAINERS_ONE_FUNCTION_EXEC_POLICY(function)
#endif

#define TWO_CONTAINERS_ONE_FUNCTION_COMBINED(function) \
  TWO_CONTAINERS_ONE_FUNCTION(function)                \
  TWO_CONTAINERS_ONE_FUNCTION_EXEC_POLICY(function)

#define TWO_CONTAINERS_ONE_VALUE(function)                                 \
  template <typename Container1, typename Container2, typename T>          \
  auto function(Container1 &&c1, Container2 &&c2, T &&val) {               \
    return function(begin(c1), end(c1), begin(c2), end(c2), const T &val); \
  }

#if defined(__cpp_lib_parallel_algorithm) && \
    __cpp_lib_parallel_algorithm >= 201603
#define TWO_CONTAINERS_ONE_VALUE_EXEC_POLICY(function)                      \
  template <typename ExecutionPolicy, typename Container1,                  \
            typename Container2, typename T,                                \
            typename std::enable_if<                                        \
                std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>>> \
  auto function(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2, \
                const T &val) {                                             \
    return function(forward<ExecutionPolicy>(policy), begin(c1), end(c1),   \
                    begin(c2), end(c2), val);                               \
  }
#else
#define TWO_CONTAINERS_ONE_VALUE_EXEC_POLICY(function)
#endif

#define TWO_CONTAINERS_ONE_VALUE_COMBINED(function) \
  TWO_CONTAINERS_ONE_FUNCTION(function)             \
  TWO_CONTAINERS_ONE_FUNCTION_EXEC_POLICY(function)

namespace std {

/*
 * Warning: non-standard
 * BinaryPredicate which compares two (possibly) different types with operator==
 */
template <typename T1, typename T2>
struct equal_predicate {
  constexpr bool operator()(const T1 &lhs, const T2 &rhs) const {
    return lhs == rhs;
  }
};

/* Non-modifying sequence operations */

#if 0
template <class Container, class UnaryPredicate>
bool all_of(Container &&c, UnaryPredicate &&p) {
  return all_of(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
bool all_of(ExecutionPolicy &&policy, Container &&c, UnaryPredicate &&p) {
  return all_of(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                forward<UnaryPredicate>(p));
}
#endif
ONE_CONTAINER_ONE_FUNCTION_COMBINED(all_of)

#if 0
template <class Container, class UnaryPredicate>
bool any_of(Container &&c, UnaryPredicate &&p) {
  return any_of(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
bool any_of(ExecutionPolicy &&policy, Container &&c, UnaryPredicate &&p) {
  return any_of(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                forward<UnaryPredicate>(p));
}
#endif
ONE_CONTAINER_ONE_FUNCTION_COMBINED(any_of)

#if 0
template <class Container, class UnaryPredicate>
bool none_of(Container &&c, UnaryPredicate &&p) {
  return none_of(c.begin(), c.end(), forward<UnaryPredicate>(p));
}


template <class ExecutionPolicy, class Container, class UnaryPredicate>
bool none_of(ExecutionPolicy &&policy, Container &&c, UnaryPredicate &&p) {
  return none_of(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                 forward<UnaryPredicate>(p));
}
#endif
ONE_CONTAINER_ONE_FUNCTION_COMBINED(none_of)

#if 0
template <class Container, class UnaryFunction>
UnaryFunction for_each(Container &&c, UnaryFunction &&f) {
  return for_each(c.begin(), c.end(), forward<UnaryFunction>(f));
}

template <class ExecutionPolicy, class Container, class UnaryFunction>
void for_each(ExecutionPolicy &&policy, Container &&c, UnaryFunction &&f) {
  for_each(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
           forward<UnaryFunction>(f));
}
#endif
ONE_CONTAINER_ONE_FUNCTION_COMBINED(for_each)

#if 0
template <class Container, class T>
auto count(Container &&c, const T &value) {
  return count(c.begin(), c.end(), value);
}

template <class ExecutionPolicy, class Container, class T>
auto count(ExecutionPolicy &&policy, Container &&c, const T &value) {
  return count(forward<ExecutionPolicy>(policy), c.begin(), c.end(), value);
}
#endif
ONE_CONTAINER_ONE_VALUE_COMBINED(count)

#if 0
template <class Container, class UnaryPredicate>
auto count_if(Container &&c, UnaryPredicate &&p) {
  return count_if(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
auto count_if(ExecutionPolicy &&policy, Container &&c, UnaryPredicate &&p) {
  return count_if(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                  forward<UnaryPredicate>(p));
}
#endif
ONE_CONTAINER_ONE_FUNCTION_COMBINED(count_if)

#if 0
/*
template <class Container1, class Container2>
auto mismatch(Container1 &&c1, Container2 &&c2) {
  return mismatch(c1.begin(), c1.end(), c2.begin(), c2.end());
}

template <class ExecutionPolicy, class Container1, class Container2>
auto mismatch(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2) {
  return mismatch(forward<ExecutionPolicy>(policy), c1.begin(), c1.end(),
                  c2.begin(), c2.end());
}
*/

template <class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto mismatch(Container1 &&c1, Container2 &&c2, BinaryPredicate &&p = {}) {
  return mismatch(c1.begin(), c1.end(), c2.begin(), c2.end(),
                  forward<BinaryPredicate>(p));
}

template <class ExecutionPolicy, class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto mismatch(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2,
              BinaryPredicate &&p = {}) {
  return mismatch(forward<ExecutionPolicy>(policy), c1.begin(), c1.end(),
                  c2.begin(), c2.end(), forward<BinaryPredicate>(p));
}
#endif
TWO_CONTAINERS_COMBINED(mismatch)
TWO_CONTAINERS_ONE_FUNCTION_COMBINED(mismatch)

#if 0
template <class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto equal(Container1 &&c1, Container2 &&c2, BinaryPredicate &&p = {}) {
  return equal(c1.begin(), c1.end(), c2.begin(), c2.end(),
               forward<BinaryPredicate>(p));
}

template <class ExecutionPolicy, class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto equal(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2,
           BinaryPredicate &&p = {}) {
  return equal(forward<ExecutionPolicy>(policy), c1.begin(), c1.end(),
               c2.begin(), c2.end(), forward<BinaryPredicate>(p));
}
#endif
TWO_CONTAINERS_ONE_FUNCTION_COMBINED(equal)

#if 0
template <class Container, class T>
auto find(Container &&c, const T &value) {
  return find(c.begin(), c.end(), value);
}

template <class ExecutionPolicy, class Container, class T>
auto find(ExecutionPolicy &&policy, Container &&c, const T &value) {
  return find(forward<ExecutionPolicy>(policy), c.begin(), c.end(), value);
}
#endif
ONE_CONTAINER_ONE_VALUE_COMBINED(find)

#if 0
template <class Container, class UnaryPredicate>
auto find_if(Container c, UnaryPredicate &&p) {
  return find_if(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
auto find_if(ExecutionPolicy &&policy, Container &&c, UnaryPredicate p) {
  return find_if(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                 forward<UnaryPredicate>(p));
}
#endif
ONE_CONTAINER_ONE_FUNCTION_COMBINED(find_if)

#if 0
template <class Container, class UnaryPredicate>
auto find_if_not(Container c, UnaryPredicate &&p) {
  return find_if_not(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
auto find_if_not(ExecutionPolicy &&policy, Container &&c, UnaryPredicate p) {
  return find_if_not(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                     forward<UnaryPredicate>(p));
}
#endif
ONE_CONTAINER_ONE_FUNCTION_COMBINED(find_if_not)

#if 0
template <class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto find_end(Container1 &&c1, Container2 &&c2, BinaryPredicate &&p = {}) {
  return find_end(c1.begin(), c1.end(), c2.begin(), c2.end(),
                  forward<BinaryPredicate>(p));
}

template <class ExecutionPolicy, class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto find_end(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2,
              BinaryPredicate &&p = {}) {
  return find_end(forward<ExecutionPolicy>(policy), c1.begin(), c1.end(),
                  c2.begin(), c2.end(), forward<BinaryPredicate>(p));
}
#endif
TWO_CONTAINERS_COMBINED(find_end)
TWO_CONTAINERS_ONE_FUNCTION_COMBINED(find_end)

#if 0
template <class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto find_first_of(Container1 &&c1, Container2 &&c2, BinaryPredicate &&p = {}) {
  return find_first_of(c1.begin(), c1.end(), c2.begin(), c2.end(),
                       forward<BinaryPredicate>(p));
}

template <class ExecutionPolicy, class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto find_first_of(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2,
                   BinaryPredicate &&p = {}) {
  return find_first_of(forward<ExecutionPolicy>(policy), c1.begin(), c1.end(),
                       c2.begin(), c2.end(), forward<BinaryPredicate>(p));
}
#endif
TWO_CONTAINERS_COMBINED(find_first_of)
TWO_CONTAINERS_ONE_FUNCTION_COMBINED(find_first_of)

#if 0
template <
    class Container,
    class BinaryPredicate = class equal_to<typename Container::value_type>>
auto adjacent_find(Container &&c, BinaryPredicate &&p = {}) {
  return adjacent_find(c.begin(), c.end(), forward<BinaryPredicate>(p));
}

template <class ExecutionPolicy, class Container,
          class BinaryPredicate = equal_to<typename Container::value_type>>
auto adjacent_find(ExecutionPolicy &&policy, Container &&c,
                   BinaryPredicate &&p = {}) {
  return adjacent_find(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                       forward<BinaryPredicate>(p));
}
#endif
TWO_CONTAINERS_COMBINED(adjacent_find)
TWO_CONTAINERS_ONE_FUNCTION_COMBINED(adjacent_find)

#if 0
template <class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto search(Container1 &&c1, Container2 &&c2, BinaryPredicate &&p = {}) {
  return search(c1.begin(), c1.end(), c2.begin(), c2.end(),
                forward<BinaryPredicate>(p));
}

template <class ExecutionPolicy, class Container1, class Container2,
          class BinaryPredicate = equal_predicate<
              typename Container1::value_type, typename Container2::value_type>>
auto search(ExecutionPolicy &&policy, Container1 &&c1, Container2 &&c2,
            BinaryPredicate &&p = {}) {
  return search(forward<ExecutionPolicy>(policy), c1.begin(), c1.end(),
                c2.begin(), c2.end(), forward<BinaryPredicate>(p));
}
#endif
TWO_CONTAINERS_COMBINED(search)
TWO_CONTAINERS_ONE_FUNCTION_COMBINED(search)

#if 0
template <class Container, class Searcher>
auto search(Container &&c, const Searcher &searcher) {
  return search(c.begin(), c.end(), searcher);
}

template <
    class Container, class Size, class T,
    class BinaryPredicate = equal_predicate<typename Container::value_type, T>>
auto search_n(Container &&c, Size count, const T &value,
              BinaryPredicate &&p = {}) {
  return search_n(c.begin(), c.end(), count, value,
                  forward<BinaryPredicate>(p));
}
#endif
TWO_CONTAINERS_ONE_VALUE_COMBINED(search_n)

template <
    class ExecutionPolicy, class Container, class Size, class T,
    class BinaryPredicate = equal_predicate<typename Container::value_type, T>>
auto search_n(ExecutionPolicy &&policy, Container &&c, Size count,
              const T &value, BinaryPredicate &&p = {}) {
  return search_n(forward<ExecutionPolicy>(policy), c.begin(), c.end(), count,
                  value, forward<BinaryPredicate>(p));
}

}  // namespace std

#endif  // INCLUDE_ALGORITHM_HPP
