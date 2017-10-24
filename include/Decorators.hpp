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
class Decorator<H, R(Args...)> {
 public:
  using function_type = R(Args...);

  constexpr Decorator() = default;

  constexpr Decorator(H hook, std::function<function_type> f) noexcept
      : d_hook{std::move(hook)}, d_function{std::move(f)} {}

  constexpr auto operator()(Args... args) {
    auto arguments = std::make_tuple(args...);
    if constexpr (!std::is_void_v<decltype(d_hook.pre(args...))>) {
      std::optional<R> candidate = d_hook.pre(args...);
      if (candidate) return std::make_pair(None{}, candidate.value());
    } else {
      d_hook.pre(args...);
    }
    /* Decorated function does not return anything */
    if constexpr (std::is_void_v<R>) {
      d_function(std::forward<Args>(args)...);
      if constexpr (!std::is_void_v<decltype(d_hook.post(None{}, arguments))>) {
        return std::make_pair(d_hook.post(None{}, arguments), None{});
      } else {  // Required else because of return type deduction
        d_hook.post(None{}, arguments);
        return std::make_pair(None{}, None{});
      }
    } else {
      R ret = d_function(std::forward<Args>(args)...);
      if constexpr (!std::is_void_v<decltype(d_hook.post(ret, arguments))>) {
        return std::make_pair(d_hook.post(ret, arguments), ret);
      } else {
        d_hook.post(ret, arguments);
        return std::make_pair(None{}, ret);
      }
    }
  }

  function_type *target() {
    return *d_function.template target<function_type *>();
  }

 private:
  H d_hook;
  std::function<function_type> d_function;
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
  constexpr MemoizerHook() = default;

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
  constexpr StatelessHook(Pre pre, Post post) noexcept
      : d_pre{std::move_if_noexcept(pre)},
        d_post{std::move_if_noexcept(post)} {}

  constexpr auto pre(Args... args) const { d_pre(args...); }

  template <typename Ret2,
            typename = std::enable_if_t<is_similar_v<Ret2, Ret> &&
                                        !std::is_void_v<Ret2>>>
  constexpr auto post(Ret2 &&returned, const std::tuple<Args...> &) const {
    return d_post(std::forward<Ret2>(returned));
  }

  constexpr auto post(None, const std::tuple<Args...> &) const {
    return d_post();
  }

  const Pre d_pre;
  const Post d_post;
};

/* https://stackoverflow.com/a/39717241/1666415 */
template <typename T, typename = void>
struct function_traits;

template <typename R, typename... A>
struct function_traits<R (*)(A...)> {
  using return_type = R;
  using class_type = void;
  using args_type = std::tuple<A...>;
};

template <typename R, typename C, typename... A>
struct function_traits<R (C::*)(A...)> {
  using return_type = R;
  using class_type = C;
  using args_type = std::tuple<A...>;
};

template <typename R, typename C, typename... A>
struct function_traits<R (C::*)(A...) const> {
  using return_type = R;
  using class_type = C;
  using args_type = std::tuple<A...>;
};

template <typename T>
struct function_traits<T, std::void_t<decltype(&T::operator())>>
    : public function_traits<decltype(&T::operator())> {};

/**
 * Allow deduction of template parameters while constructing a decorator
 */
template <typename Hook, typename Callable, typename R>
struct decorator_maker {
  Hook d_h;
  Callable d_f;

  constexpr decorator_maker(Hook h, Callable f) noexcept
      : d_h{std::move_if_noexcept(h)}, d_f{std::move_if_noexcept(f)} {}

  template <typename... Args, typename = std::enable_if_t<
                                  std::is_invocable_r_v<R, Callable, Args...>>>
  constexpr auto operator()(Args...) noexcept {
    return Decorator<decltype(d_h), R(Args...)>(std::move(d_h), std::move(d_f));
  }
};

template <typename Hook, typename Callable>
constexpr auto make_decorator(Hook hook, Callable f) noexcept {
  using traits = function_traits<Callable>;
  constexpr auto tup = typename traits::args_type{};
  decorator_maker<Hook, Callable, typename traits::return_type> maker{
      std::move(hook), std::move(f)};
  return std::apply(std::move(maker), tup);
}

template <typename Hook, typename R, typename... Args,
          typename = std::enable_if_t<std::is_function_v<R(Args...)>>>
constexpr auto make_decorator(Hook hook, R (*f)(Args...)) noexcept {
  return Decorator<Hook, R(Args...)>{std::move(hook), f};
}

template <
    typename Hook, typename Callable, typename R, typename... Args,
    typename = std::enable_if_t<std::is_invocable_r_v<R, Callable, Args...>>>
constexpr auto make_decorator(Hook hook, Callable f) noexcept {
  return Decorator<Hook, R(Args...)>{std::move(hook),
                                     std::function<R(Args...)>{f}};
}

template <typename Hook, typename R, typename... Args>
constexpr auto make_decorator(Hook hook, std::function<R(Args...)> f) noexcept {
  return Decorator<Hook, R(Args...)>{std::move(hook), std::move(f)};
}

template <typename R, typename... Args>
constexpr auto make_memoized(R (*f)(Args...)) noexcept {
  auto hook = MemoizerHook<R, Args...>{};
  return make_decorator(std::move(hook), f);
}

template <typename Callable, typename R>
struct memoized_maker {
  Callable d_f;

  constexpr memoized_maker(Callable f) noexcept : d_f{std::move(f)} {}

  template <typename... Args>
  constexpr auto operator()(Args...) noexcept {
    auto hook = MemoizerHook<R, Args...>{};
    return make_decorator<decltype(hook), decltype(d_f), R, Args...>(
        std::move(hook), std::move(d_f));
  }
};

template <typename Callable>
constexpr auto make_memoized(Callable f) noexcept {
  using traits = function_traits<Callable>;
  memoized_maker<Callable, typename traits::return_type> mm{f};
  auto tup = typename traits::args_type{};
  return std::apply(std::move(mm), tup);
}

template <typename R, typename... Args>
constexpr auto make_memoized(std::function<R(Args...)> f) noexcept {
  auto hook = MemoizerHook<R, Args...>{};
  return make_decorator(std::move(hook), std::move(f));
}

#endif  // INCLUDE_DECORATORS_HPP_
