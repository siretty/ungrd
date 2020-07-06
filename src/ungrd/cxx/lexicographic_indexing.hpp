#ifndef UNGRD_LEXICOGRAPHIC_INDEXING_HPP_6F86FA3F5DD1436DBB3EA110B6A5A6C5
#define UNGRD_LEXICOGRAPHIC_INDEXING_HPP_6F86FA3F5DD1436DBB3EA110B6A5A6C5

#include <array>
#include <optional>

#include <cstddef>

namespace ungrd {

template <size_t NDim>
class lexicographic_indexing {
public:
  static constexpr size_t ndim = NDim;

  using index_type = size_t;
  using ndidx_type = std::array<size_t, ndim>;

public:
  constexpr index_type encode(ndidx_type const ndidx) const {
    index_type index = 0;
    for (size_t dim = 0; dim < ndim; ++dim)
      index += ndidx[dim] * strides_[dim];
    return index;
  }

  constexpr std::optional<index_type> try_encode(ndidx_type const ndidx) const {
    index_type index = 0;
    for (size_t dim = 0; dim < ndim; ++dim) {
      if (ndidx[dim] >= extents_[dim])
        return std::nullopt;
      index += ndidx[dim] * strides_[dim];
    }
    return index;
  }

  // TODO: rename encode -> raw_encode, try_encode -> encode

  constexpr ndidx_type decode(index_type index) const {
    ndidx_type ndidx;
    if constexpr (ndim > 0) {
      for (size_t dim = ndim - 1; dim >= 1; --dim) {
        ndidx[dim] = index % extents_[dim];
        index = index / extents_[dim];
      }
      ndidx[0] = index;
    }
    return ndidx;
  }

  // TODO: rename decode -> raw_decode, ...

public:
  constexpr auto const extents() const { return extents_; }

  constexpr size_t extent(size_t const dim) const { return extents_[dim]; }

  constexpr size_t const size() const { return size_; }

private:
  constexpr void _update_internals() {
    size_ = 1;
    for (size_t dim = 0; dim < ndim; ++dim)
      size_ *= extents_[dim];

    if constexpr (ndim > 0) {
      strides_[ndim - 1] = 1;
      for (size_t dim = ndim - 1; dim >= 1; --dim) {
        strides_[dim - 1] = extents_[dim] * strides_[dim];
      }
    }
  }

public:
  lexicographic_indexing() = default;

  explicit lexicographic_indexing(std::array<size_t, ndim> _extents)
      : extents_{_extents} {
    _update_internals();
  }

private:
  std::array<size_t, ndim> extents_ = {};
  std::array<size_t, ndim> strides_ = {};
  size_t size_ = 0;
};

} // namespace ungrd

#endif // UNGRD_LEXICOGRAPHIC_INDEXING_HPP_6F86FA3F5DD1436DBB3EA110B6A5A6C5
