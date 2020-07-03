#ifndef UNGRD_NDARRAY_HPP_363A4707D1F6487E8BCB789B57214F67
#define UNGRD_NDARRAY_HPP_363A4707D1F6487E8BCB789B57214F67

#include <array>
#include <optional>

#include <cstddef>

namespace ungrd {

template <size_t ndim>
constexpr size_t nd_element_count(std::array<size_t, ndim> const &extents) {
  size_t count = 1;
  for (size_t dim = 0; dim < ndim; ++dim)
    count *= extents[dim];
  return count;
}

template <size_t ndim>
constexpr std::array<size_t, ndim>
nd_strides(std::array<size_t, ndim> const &extents) {
  if constexpr (ndim == 0)
    return {};

  std::array<size_t, ndim> strides;
  strides[ndim - 1] = 1;
  for (ptrdiff_t dim = ndim - 2; dim >= 0; --dim) {
    strides[dim] = extents[dim + 1] * strides[dim + 1];
  }

  return strides;
}

template <size_t ndim>
constexpr size_t nd_linear_index(
    std::array<size_t, ndim> const &indices,
    std::array<size_t, ndim> const &strides) {
  size_t linear_index = 0;
  for (size_t dim = 0; dim < ndim; ++dim)
    linear_index += indices[dim] * strides[dim];
  return linear_index;
}

template <size_t ndim>
constexpr std::array<size_t, ndim>
nd_indices(size_t linear_index, std::array<size_t, ndim> const &extents) {
  std::array<size_t, ndim> indices;
  if constexpr (ndim > 0) {
    for (size_t dim = ndim - 1; dim >= 1; --dim) {
      indices[dim] = linear_index % extents[dim];
      linear_index = linear_index / extents[dim];
    }
    indices[0] = linear_index;
  }
  return indices;
}

} // namespace ungrd

#endif // UNGRD_NDARRAY_HPP_363A4707D1F6487E8BCB789B57214F67
