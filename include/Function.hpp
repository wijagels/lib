// Copyright 2018 William Jagels
#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <variant>

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

  virtual std::unique_ptr<function_storage> clone() const = 0;
  virtual void clone_into(function_storage *) const = 0;
  virtual void move_into(function_storage *) noexcept = 0;
  virtual ResultType invoke(ArgumentTypes &&... args) = 0;
};

template <typename FunctionType, typename ResultType, typename... ArgumentTypes>
struct function_storage_impl : function_storage<ResultType, ArgumentTypes...> {
  function_storage_impl(FunctionType fn) : m_fn{fn} {}

  std::unique_ptr<function_storage<ResultType, ArgumentTypes...>> clone()
      const override {
    return std::make_unique<function_storage_impl>(m_fn);
  }

  void clone_into(
      function_storage<ResultType, ArgumentTypes...> *ptr) const override {
    new (ptr) function_storage_impl(*this);
  }

  void move_into(
      function_storage<ResultType, ArgumentTypes...> *ptr) noexcept override {
    new (ptr) function_storage_impl(std::move(*this));
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
  static constexpr std::size_t MAX_SIZE = 24;
  static constexpr std::size_t ALIGNMENT = 8;

  using ptr_t = typename std::unique_ptr<
      detail::function_storage<ResultType, ArgumentTypes...>>;
  using inline_storage = std::aligned_storage_t<MAX_SIZE, ALIGNMENT>;
  using base_fn_storage =
      detail::function_storage<ResultType, ArgumentTypes...>;

  std::variant<std::monostate, inline_storage, ptr_t> m_storage;

 public:
  constexpr Function() = default;
  ~Function() {
    if (auto *ptr = std::get_if<inline_storage>(&m_storage))
      std::destroy_at(reinterpret_cast<base_fn_storage *>(ptr));
  }

  template <typename FunctionType,
            typename = std::enable_if_t<std::is_invocable_r_v<
                ResultType, FunctionType, ArgumentTypes...>>>
  Function(FunctionType f) {
    if constexpr (sizeof(detail::function_storage_impl<FunctionType, ResultType,
                                                       ArgumentTypes...>) <=
                      MAX_SIZE &&
                  alignof(
                      detail::function_storage_impl<FunctionType, ResultType,
                                                    ArgumentTypes...>) <=
                      ALIGNMENT) {
      m_storage.template emplace<inline_storage>();
      auto *ptr = std::get_if<inline_storage>(&m_storage);
      new (ptr) detail::function_storage_impl<FunctionType, ResultType,
                                              ArgumentTypes...>{std::move(f)};
    } else {
      m_storage.template emplace<ptr_t>(
          std::make_unique<detail::function_storage_impl<
              FunctionType, ResultType, ArgumentTypes...>>(std::move(f)));
    }
  }

  Function(const Function &other) {
    if (auto ptr = std::get_if<ptr_t>(&other.m_storage))
      m_storage = (*ptr)->clone();
    else if (auto inline_ptr = std::get_if<inline_storage>(&other.m_storage)) {
      m_storage.template emplace<inline_storage>();
      reinterpret_cast<const base_fn_storage *>(inline_ptr)
          ->clone_into(reinterpret_cast<base_fn_storage *>(
              std::get_if<inline_storage>(&m_storage)));
    }
  }

  Function(Function &&other) noexcept {
    if (auto ptr = std::get_if<ptr_t>(&other.m_storage))
      m_storage = std::move(*ptr);
    else if (auto inline_ptr = std::get_if<inline_storage>(&other.m_storage)) {
      m_storage.template emplace<inline_storage>();
      reinterpret_cast<base_fn_storage *>(inline_ptr)
          ->move_into(reinterpret_cast<base_fn_storage *>(
              std::get_if<inline_storage>(&m_storage)));
    }
  }

  Function &operator=(const Function &other) {
    if (this != &other) {
      if (auto ptr = std::get_if<ptr_t>(&other.m_storage))
        m_storage = (*ptr)->clone();
      else if (auto inline_ptr =
                   std::get_if<inline_storage>(&other.m_storage)) {
        m_storage.template emplace<inline_storage>();
        reinterpret_cast<const base_fn_storage *>(inline_ptr)
            ->clone_into(reinterpret_cast<base_fn_storage *>(
                std::get_if<inline_storage>(&m_storage)));
      }
    }
    return *this;
  }

  Function &operator=(Function &&other) noexcept {
    if (this != &other) {
      if (auto ptr = std::get_if<ptr_t>(&other.m_storage))
        m_storage = std::move(*ptr);
      else if (auto inline_ptr =
                   std::get_if<inline_storage>(&other.m_storage)) {
        m_storage.template emplace<inline_storage>();
        reinterpret_cast<base_fn_storage *>(inline_ptr)
            ->move_into(reinterpret_cast<base_fn_storage *>(
                std::get_if<inline_storage>(&m_storage)));
      }
    }
    return *this;
  }

  ResultType operator()(ArgumentTypes &&... args) {
    if (auto ptr = std::get_if<ptr_t>(&m_storage))
      return (*ptr)->invoke(std::forward<ArgumentTypes>(args)...);
    else if (auto inline_ptr = std::get_if<inline_storage>(&m_storage))
      return reinterpret_cast<base_fn_storage *>(inline_ptr)
          ->invoke(std::forward<ArgumentTypes>(args)...);
    else
      throw BadFunctionCall{"No function stored"};
  }

  constexpr explicit operator bool() const { return m_storage.index() != 0; }

  constexpr friend bool operator==(
      const Function<ResultType(ArgumentTypes...)> &f, std::nullptr_t) {
    return !bool{f};
  }
  constexpr friend bool operator==(
      std::nullptr_t, const Function<ResultType(ArgumentTypes...)> &f) {
    return !bool{f};
  }
  constexpr friend bool operator!=(
      const Function<ResultType(ArgumentTypes...)> &f, std::nullptr_t) {
    return bool{f};
  }
  constexpr friend bool operator!=(
      std::nullptr_t, const Function<ResultType(ArgumentTypes...)> &f) {
    return bool{f};
  }
};

}  // namespace wijagels
