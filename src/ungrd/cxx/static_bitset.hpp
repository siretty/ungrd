#ifndef UNGRD_STATIC_BITSET_HPP_8B94BD5EF391463FACAE08425F74A0F0
#define UNGRD_STATIC_BITSET_HPP_8B94BD5EF391463FACAE08425F74A0F0

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <iterator>
#include <limits>

#include <cassert>

namespace ungrd {

template <size_t NBits, std::unsigned_integral TChunk = size_t>
class static_bitset {
  using chunk_type = TChunk;

  static constexpr size_t chunk_bits = sizeof(chunk_type) * 8;
  static constexpr size_t chunk_count = (NBits / chunk_bits) + 1;

  static constexpr chunk_type last_chunk_mask = [] {
    size_t const bit_count = NBits % chunk_bits;

    chunk_type mask = 0;
    for (size_t bit_index = 0; bit_index < bit_count; ++bit_index)
      mask |= (size_t{1} << bit_index);

    return mask;
  }();

  static constexpr chunk_type chunk_all_set =
      std::numeric_limits<chunk_type>::max();

  constexpr static auto get_bit_position(size_t const index) {
    size_t const chunk_index = index / chunk_bits;
    size_t const bit_index = index % chunk_bits;
    assert(chunk_index < chunk_count);
    return std::make_pair(chunk_index, bit_index);
  }

  constexpr static chunk_type get_chunk_mask(size_t const bit_index) {
    return chunk_type{1} << bit_index;
  }

public:
  constexpr size_t size() const { return NBits; }

public:
  constexpr bool all() const {
    bool full_chunks =
        std::all_of(chunks_.begin(), std::prev(chunks_.end()), [](auto &chunk) {
          return chunk == chunk_all_set;
        });

    if (not full_chunks)
      return false;

    auto const last_chunk = chunks_.back();
    return (last_chunk & last_chunk_mask) == (chunk_all_set & last_chunk_mask);
  }

  constexpr bool any() const {
    bool full_chunks =
        std::any_of(chunks_.begin(), std::prev(chunks_.end()), [](auto &chunk) {
          return chunk != 0;
        });

    if (full_chunks)
      return true;

    auto const last_chunk = chunks_.back();
    return (last_chunk & last_chunk_mask) != 0;
  }

  constexpr bool none() const {
    return std::all_of(
        chunks_.begin(), chunks_.end(), [](auto &chunk) { return chunk == 0; });
  }

public:
  constexpr size_t count_set_bits() const {
    size_t count = 0;
    std::for_each(
        chunks_.begin(), std::prev(chunks_.end()),
        [&count](auto const chunk) { count += std::popcount(chunk); });
    count += std::popcount(chunks_.back() & last_chunk_mask);
    return count;
  }

public:
  constexpr bool get(size_t const index) const {
    auto [chunk_index, bit_index] = get_bit_position(index);
    auto const mask = get_chunk_mask(bit_index);
    return chunks_[chunk_index] & mask;
  }

public:
  constexpr void set_to(size_t const index, bool const value) {
    auto [chunk_index, bit_index] = get_bit_position(index);
    auto const mask = get_chunk_mask(bit_index);
    if (value)
      chunks_[chunk_index] |= mask;
    else
      chunks_[chunk_index] &= ~mask;
  }

public:
  constexpr void set(size_t const index) { set_to(index, true); }

  constexpr void reset(size_t const index) { set_to(index, false); }

public:
  constexpr void set_all_to(bool const value = true) {
    chunks_.fill(value ? chunk_all_set : 0);
  }

public:
  constexpr void set_all() { set_all_to(true); }

  constexpr void reset_all() { set_all_to(false); }

public:
  constexpr size_t countr_zero() const {
    size_t result = 0;
    for (size_t chunk_index = 0; chunk_index < chunks_.size(); ++chunk_index) {
      auto const chunk = chunks_[chunk_index];
      auto const chunk_first_set = std::countr_zero(chunk);
      result += chunk_first_set;
      if (chunk_first_set < chunk_bits)
        break;
    }
    return std::min(result, NBits);
  }

  constexpr size_t countr_one() const {
    size_t result = 0;
    for (size_t chunk_index = 0; chunk_index < chunks_.size(); ++chunk_index) {
      auto const chunk = chunks_[chunk_index];
      auto const chunk_first_set = std::countr_one(chunk);
      result += chunk_first_set;
      if (chunk_first_set < chunk_bits)
        break;
    }
    return std::min(result, NBits);
  }

private:
  std::array<chunk_type, chunk_count> chunks_ = {};
};

} // namespace ungrd

#endif // UNGRD_STATIC_BITSET_HPP_8B94BD5EF391463FACAE08425F74A0F0
