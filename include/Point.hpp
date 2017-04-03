#ifndef POINT_HPP
#define POINT_HPP

#include <array>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

template <size_t D, typename T>
class Point;

/*
 * Point of dimension D and coordinate type T.
 * Coordinate type defaults to double.
 */
template <size_t D, typename T = double>
class Point {
 public:
  /*
   * Create a point with default constructed coordinates.
   * Is disabled if T is not default constructible.
   */
  template <typename = typename std::enable_if<
                std::is_default_constructible<T>::value>::type>
  Point() : coordinates_{} {
    coordinates_.fill(T{});
  }

  Point(const std::initializer_list<T>& v) {
    if (v.size() != D)
      throw std::length_error{"Initializer list is incorrectly sized"};
    std::copy(v.begin(), v.end(), coordinates_.begin());
  }

  explicit Point(const std::array<T, D>& arr) : coordinates_{arr} {}

  /*
   * No bounds check, returns by value
   */
  T operator[](size_t dimension) { return coordinates_[dimension]; }

  const T operator[](size_t dimension) const { return coordinates_[dimension]; }

  /*
   * Bounds checked, returns reference
   */
  T& at(size_t dimension) { return coordinates_.at(dimension); }

  const T& at(size_t dimension) const { return coordinates_.at(dimension); }

  Point<D>& operator=(const Point& other) {
    this->coordinates_ = other.coordinates_;
    return *this;
  }

  /*
   * Projects the point orthogonally onto a new point of dimension DIMEN_TO.
   * If DIMEN_TO is larger than D, the remainder of the resulting Point will
   * be filled with default constructed instances of T.
   * This function will fail if T is not default constructible.
   */
  template <size_t DIMEN_TO>
  Point<DIMEN_TO> orthogonal_projection() const {
    std::array<T, DIMEN_TO> arr{};
    arr.fill(T{});
    // Copy up to current dimension or DIMEN_TO, whichever is smallest
    std::copy(coordinates_.begin(),
              coordinates_.begin() + std::min(D, DIMEN_TO), arr.begin());
    return Point<DIMEN_TO, T>{arr};
  }

  friend bool operator==(const Point<D>& lhs, const Point<D>& rhs) {
    return lhs.coordinates_ == rhs.coordinates_;
  }

  friend bool operator!=(const Point<D>& lhs, const Point<D>& rhs) {
    return lhs.coordinates_ != rhs.coordinates_;
  }

 private:
  std::array<T, D> coordinates_;
};

/*
 * Finds euclidian distance between two Point classes of same dimension
 */
template <size_t D, typename T>
T distance(const Point<D, T>& lhs, const Point<D>& rhs) {
  T dist{};
  for (size_t i = 0; i < D; i++) dist += std::pow(lhs[i] - rhs[i], 2);
  return std::sqrt(dist);
}

/*
 * Finds euclidian distance between Point and axis
 */
template <size_t D, typename T>
T distance(const Point<D, T>& point, const T& value, int axis) {
  if (axis < 0 && axis > D) throw std::out_of_range{"Invalid axis"};
  return std::abs(point[axis] - value);
}

/*
 * Computes manhattan distance of two points that may have different coordinate
 * storage types.
 */
template <size_t D, typename T1, typename T2>
decltype(T1{} + T2{}) manhattan_distance(const Point<D, T1>& lhs,
                                         const Point<D, T2>& rhs) {
  auto sum = decltype(T1{} + T2{}){};

  for (size_t i = 0; i != D; ++i) {
    sum += std::abs(lhs[i] - rhs[i]);
  }

  return sum;
}

/*
 * Print the point components of a Point in ascending order.
 * Prints in the form {x, y, z} with no newline.
 */
template <size_t D>
std::ostream& operator<<(std::ostream& os, const Point<D>& point) {
  os << "{";
  for (size_t i = 0; i < D - 1; i++) {
    os << point[i] << ", ";
  }
  return os << point[D - 1] << "}";
}

namespace std {
/*
 * Make our Point class hashable
 */
template <size_t D>
struct hash<Point<D>> {
  size_t operator()(const Point<D>& x) const {
    return hash<array<double, D>>()(x.coordinates_);
  }
};

/*
 * Also make it swappable for containers
 */
template <size_t D>
void swap(Point<D>& lhs, Point<D>& rhs) {
  swap(lhs.coordinates_, rhs.coordinates_);
}
}  // namespace std

#endif  // POINT_HPP
