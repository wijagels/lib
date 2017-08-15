// Copyright 2017 William Jagels
#ifndef INCLUDE_ALGORITHM_HPP_
#define INCLUDE_ALGORITHM_HPP_
/*
 * A set of extensions to <algorithm>
 */

#include <algorithm>
#include <utility>

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

template <class Container, class UnaryPredicate>
bool all_of(Container &&c, UnaryPredicate &&p) {
  return all_of(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
bool all_of(ExecutionPolicy &&policy, Container &&c, UnaryPredicate &&p) {
  return all_of(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                forward<UnaryPredicate>(p));
}

template <class Container, class UnaryPredicate>
bool any_of(Container &&c, UnaryPredicate &&p) {
  return any_of(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
bool any_of(ExecutionPolicy &&policy, Container &&c, UnaryPredicate &&p) {
  return any_of(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                forward<UnaryPredicate>(p));
}

template <class Container, class UnaryPredicate>
bool none_of(Container &&c, UnaryPredicate &&p) {
  return none_of(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
bool none_of(ExecutionPolicy &&policy, Container &&c, UnaryPredicate &&p) {
  return none_of(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                 forward<UnaryPredicate>(p));
}

template <class Container, class UnaryFunction>
UnaryFunction for_each(Container &&c, UnaryFunction &&f) {
  return for_each(c.begin(), c.end(), forward<UnaryFunction>(f));
}

template <class ExecutionPolicy, class Container, class UnaryFunction>
void for_each(ExecutionPolicy &&policy, Container &&c, UnaryFunction &&f) {
  for_each(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
           forward<UnaryFunction>(f));
}

template <class Container, class T>
auto count(Container &&c, const T &value) {
  return count(c.begin(), c.end(), value);
}

template <class ExecutionPolicy, class Container, class T>
auto count(ExecutionPolicy &&policy, Container &&c, const T &value) {
  return count(forward<ExecutionPolicy>(policy), c.begin(), c.end(), value);
}

template <class Container, class UnaryPredicate>
auto count_if(Container &&c, UnaryPredicate &&p) {
  return count_if(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
auto count_if(ExecutionPolicy &&policy, Container &&c, UnaryPredicate &&p) {
  return count_if(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                  forward<UnaryPredicate>(p));
}

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

template <class Container, class T>
auto find(Container &&c, const T &value) {
  return find(c.begin(), c.end(), value);
}

template <class ExecutionPolicy, class Container, class T>
auto find(ExecutionPolicy &&policy, Container &&c, const T &value) {
  return find(forward<ExecutionPolicy>(policy), c.begin(), c.end(), value);
}

template <class Container, class UnaryPredicate>
auto find_if(Container c, UnaryPredicate &&p) {
  return find_if(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
auto find_if(ExecutionPolicy &&policy, Container &&c, UnaryPredicate p) {
  return find_if(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                 forward<UnaryPredicate>(p));
}

template <class Container, class UnaryPredicate>
auto find_if_not(Container c, UnaryPredicate &&p) {
  return find_if_not(c.begin(), c.end(), forward<UnaryPredicate>(p));
}

template <class ExecutionPolicy, class Container, class UnaryPredicate>
auto find_if_not(ExecutionPolicy &&policy, Container &&c, UnaryPredicate p) {
  return find_if_not(forward<ExecutionPolicy>(policy), c.begin(), c.end(),
                     forward<UnaryPredicate>(p));
}

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

}  // namespace std

#endif  // INCLUDE_ALGORITHM_HPP
