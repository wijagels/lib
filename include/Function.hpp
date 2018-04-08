// Copyright 2018 William Jagels
#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>

namespace wijagels {
struct BadFunctionCall : std::runtime_error {
  using std::runtime_error::runtime_error;
};

namespace detail {
template <typename ResultType, typename... ArgumentTypes>
struct function_storage {
  function_storage() = default;
  function_storage(const function_storage &) = default;
  function_storage(function_storage &&) noexcept = default;
  virtual ~function_storage() = default;
  function_storage &operator=(const function_storage &) = default;
  function_storage &operator=(function_storage &&) noexcept = default;

  virtual std::shared_ptr<function_storage> clone() = 0;
  virtual ResultType invoke(ArgumentTypes &&... args) = 0;
};

template <typename FunctionType, typename ResultType, typename... ArgumentTypes>
struct function_storage_impl : function_storage<ResultType, ArgumentTypes...> {
  function_storage_impl(FunctionType fn) : m_fn{fn} {}

  std::shared_ptr<function_storage<ResultType, ArgumentTypes...>> clone()
      override {
    return std::make_shared<function_storage_impl>(m_fn);
  }
  ResultType invoke(ArgumentTypes &&... args) override {
    return m_fn(std::forward<ArgumentTypes>(args)...);
  }

  FunctionType m_fn;
};
}  // namespace detail

template <typename>
class Function;

template <typename ResultType, typename... ArgumentTypes>
class Function<ResultType(ArgumentTypes...)> {
  std::shared_ptr<detail::function_storage<ResultType, ArgumentTypes...>>
      m_storage;

 public:
  Function() = default;

  template <typename FunctionType,
            typename = std::enable_if_t<std::is_invocable_r_v<
                ResultType, FunctionType, ArgumentTypes...>>>
  Function(FunctionType f) {
    m_storage = std::make_shared<detail::function_storage_impl<
        FunctionType, ResultType, ArgumentTypes...>>(std::move(f));
  }

  Function(const Function &other) {
    if (bool{other}) m_storage = other.m_storage->clone();
  }
  Function(Function &&) noexcept = default;

  Function &operator=(const Function &other) {
    if (this != &other && bool{other}) m_storage = other.m_storage->clone();
    return *this;
  }
  Function &operator=(Function &&) noexcept = default;

  ResultType operator()(ArgumentTypes &&... args) {
    if (!bool{*this}) throw BadFunctionCall{"No function stored"};
    return m_storage->invoke(std::forward<ArgumentTypes>(args)...);
  }

  explicit operator bool() const { return bool{m_storage}; }

  friend bool operator==(const Function<ResultType(ArgumentTypes...)> &f,
                         std::nullptr_t) {
    return f.m_storage == nullptr;
  }
  friend bool operator==(std::nullptr_t,
                         const Function<ResultType(ArgumentTypes...)> &f) {
    return f.m_storage == nullptr;
  }
  friend bool operator!=(const Function<ResultType(ArgumentTypes...)> &f,
                         std::nullptr_t) {
    return f.m_storage != nullptr;
  }
  friend bool operator!=(std::nullptr_t,
                         const Function<ResultType(ArgumentTypes...)> &f) {
    return f.m_storage != nullptr;
  }
};

}  // namespace wijagels
