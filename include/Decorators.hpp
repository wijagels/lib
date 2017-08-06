// Copyright 2017 William Jagels
#ifndef INCLUDE_DECORATORS_HPP_
#define INCLUDE_DECORATORS_HPP_
#include <chrono>
#include <functional>
#include <iostream>

template <typename, typename>
struct Decorator;

struct DecoratorHook {
  virtual void pre() {}
  virtual void post() {}
};

template <typename H, typename R, typename... Args>
struct Decorator<R(Args...), H> {
  Decorator(std::function<R(Args...)> f, const H &hook)
      : f_{std::move(f)}, hook_{hook} {}

  R operator()(Args... args) {
    // std::chrono::time_point<std::chrono::system_clock> start, end;
    // start = std::chrono::system_clock::now();
    hook_.pre();
    R ret = f_(args...);
    hook_.post();
    // end = std::chrono::system_clock::now();
    // std::chrono::nanoseconds elapsed_nanos =
    //     std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    // std::cout << "Execution took " << elapsed_nanos.count() << "ns"
    //           << std::endl;
    return ret;
  }
  std::function<R(Args...)> f_;

  H hook_;
};

/*
 * Partial specialization for void return values.
 */
template <typename H, typename... Args>
struct Decorator<void(Args...), H> {
  Decorator(std::function<void(Args...)> f, const H &hook)
      : f_{std::move(f)}, hook_{hook} {}

  void operator()(Args... args) {
    hook_.pre();
    f_(args...);
    hook_.post();
  }
  std::function<void(Args...)> f_;

  H hook_;
};

struct TimerHook {
  TimerHook() = default;

  void pre() { start_time_ = std::chrono::system_clock::now(); }

  void post() {
    auto end = std::chrono::system_clock::now();
    std::chrono::nanoseconds elapsed_nanos =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_time_);
    std::cout << "Execution took " << elapsed_nanos.count() << "ns"
              << std::endl;
  }

  std::chrono::time_point<std::chrono::system_clock> start_time_;
};

template <typename R, typename... Args>
Decorator<R(Args...), TimerHook> makeDecorator(R (*f)(Args...)) {
  return Decorator<R(Args...), TimerHook>{std::function<R(Args...)>(f),
                                          TimerHook{}};
}

#endif  // INCLUDE_DECORATORS_HPP_
