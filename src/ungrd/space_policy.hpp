#ifndef UNGRD_SPACE_POLICY_HPP_C156B753F8AE4F3880370E3FC531A979
#define UNGRD_SPACE_POLICY_HPP_C156B753F8AE4F3880370E3FC531A979

#include <array>
#include <limits>

#include <cstddef>
#include <cstdint>

namespace ungrd {

template <typename TIndex, std::size_t NDim>
struct space_policy {
  static constexpr std::size_t ndim = NDim;

  using position = std::array<TIndex, NDim>;

  static constexpr position most_positive_position() {
    position result;
    result.fill(std::numeric_limits<TIndex>::max());
    return result;
  }

  static constexpr position most_negative_position() {
    position result;
    result.fill(std::numeric_limits<TIndex>::lowest());
    return result;
  }
};

template <std::size_t NDim>
using s32_space_policy = space_policy<std::int32_t, NDim>;

template <std::size_t NDim>
using s64_space_policy = space_policy<std::int64_t, NDim>;

} // namespace ungrd

#endif // UNGRD_SPACE_POLICY_HPP_C156B753F8AE4F3880370E3FC531A979
