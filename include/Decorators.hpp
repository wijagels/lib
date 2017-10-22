// Copyright 2017 William Jagels
#ifndef INCLUDE_DECORATORS_HPP_
#define INCLUDE_DECORATORS_HPP_
#include <chrono>
#include <functional>
#include <iostream>
#include <optional>
#include <type_traits>
#include <unordered_map>

template <typename T, typename U>
struct is_similar
    : std::is_same<std::remove_reference_t<T>, std::remove_reference_t<U>> {};

template <class T, class U>
inline constexpr bool is_similar_v = is_similar<T, U>::value;

struct None {};

template <typename, typename>
class Decorator;

template <typename H, typename R, typename... Args>
class Decorator<R(Args...), H> {
 public:
  using function_type = R(Args...);

  constexpr Decorator() = default;

  constexpr Decorator(std::function<function_type> f, const H &hook)
      : d_function{std::move(f)}, d_hook{std::move(hook)} {}

  constexpr auto operator()(Args... args) {
    auto arguments = std::make_tuple(args...);
    if constexpr (!std::is_void_v<decltype(d_hook.pre(args...))>) {
      std::optional<R> candidate = d_hook.pre(args...);
      if (candidate) return std::make_tuple(None{}, candidate.value());
    } else {
      d_hook.pre(args...);
    }
    /* Decorated function does not return anything */
    if constexpr (std::is_void_v<R>) {
      d_function(std::forward<Args>(args)...);
      if constexpr (!std::is_void_v<decltype(d_hook.post(None{}, arguments))>) {
        return std::make_tuple(d_hook.post(None{}, arguments), None{});
      } else {  // Required else because of return type deduction
        d_hook.post(None{}, arguments);
        return std::make_tuple(None{}, None{});
      }
    } else {
      R ret = d_function(std::forward<Args>(args)...);
      if constexpr (!std::is_void_v<decltype(d_hook.post(ret, arguments))>) {
        return std::make_tuple(d_hook.post(ret, arguments), ret);
      } else {
        d_hook.post(ret, arguments);
        return std::make_tuple(None{}, ret);
      }
    }
  }

  function_type *target() {
    return *d_function.template target<function_type *>();
  }

 private:
  std::function<function_type> d_function;
  H d_hook;
};

template <typename Ret, typename... Args>
struct TimerHook {
  TimerHook() = default;

  auto pre(Args...) { d_start_time = std::chrono::system_clock::now(); }

  template <typename Ret2,
            typename = std::enable_if_t<is_similar_v<Ret2, Ret> &&
                                        !std::is_void_v<Ret2>>>
  auto post(Ret2 &&, std::tuple<Args...>) {
    auto end = std::chrono::system_clock::now();
    std::chrono::nanoseconds elapsed_nanos =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end -
                                                             d_start_time);
    std::cout << "Execution took " << elapsed_nanos.count() << "ns"
              << std::endl;
  }

  auto post(None, std::tuple<Args...>) {
    auto end = std::chrono::system_clock::now();
    std::chrono::nanoseconds elapsed_nanos =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end -
                                                             d_start_time);
    std::cout << "Execution took " << elapsed_nanos.count() << "ns"
              << std::endl;
  }

  std::chrono::time_point<std::chrono::system_clock> d_start_time;
};

template <typename Ret, typename... Args>
struct MemoizerHook {
  MemoizerHook() = default;

  std::optional<Ret> pre(Args... args) {
    auto iter = d_cache.find(std::make_tuple(args...));
    if (iter != d_cache.end()) {
      return {iter->second};
    }
    return {};
  }

  void post(const Ret &r, const std::tuple<Args...> &args) {
    d_cache[args] = r;
  }

  struct hash {
    static inline size_t hash_combiner(size_t seed, size_t other) {
      seed ^= (other + 0x9e3779b9 + (seed << 6) + (seed >> 2));
      return seed;
    }

    struct variadic_hasher {
      template <typename T, typename... Rest>
      size_t operator()(const T &v, Rest... rest) {
        size_t seed = std::hash<T>()(v);
        if constexpr (sizeof...(rest) == 0)
          return seed;
        else
          return hash_combiner(seed, operator()(rest...));
      }
    };

    size_t operator()(const std::tuple<Args...> &s) const {
      return std::apply(variadic_hasher{}, s);
    }
  };

  std::unordered_map<std::tuple<Args...>, Ret, hash> d_cache;
};

template <typename Pre, typename Post, typename Ret, typename... Args>
struct StatelessHook {
  constexpr StatelessHook(Pre pre, Post post) : d_pre{pre}, d_post{post} {}

  constexpr auto pre(Args... args) { d_pre(args...); }

  template <typename Ret2,
            typename = std::enable_if_t<is_similar_v<Ret2, Ret> &&
                                        !std::is_void_v<Ret2>>>
  constexpr auto post(Ret2 &&returned, const std::tuple<Args...> &) {
    return d_post(std::forward<Ret2>(returned));
  }

  constexpr auto post(None, const std::tuple<Args...> &) { return d_post(); }

  Pre d_pre;
  Post d_post;
};

/**
 * Allow deduction of template parameters while constructing a decorator
 */
template <typename Hook, typename R, typename... Args,
          typename = std::enable_if_t<std::is_function_v<R(Args...)>>>
constexpr auto make_decorator(Hook hook, R (*f)(Args...)) {
  return Decorator<R(Args...), Hook>{f, std::move(hook)};
}

template <
    typename Hook, typename Callable, typename R, typename... Args,
    typename = std::enable_if_t<std::is_invocable_r_v<R, Callable, Args...>>>
constexpr auto make_decorator(Hook hook, Callable f) {
  return Decorator<R(Args...), Hook>{std::function<R(Args...)>{f},
                                     std::move(hook)};
}

#endif  // INCLUDE_DECORATORS_HPP_
