#ifndef UNGRD_CELL_POLICY_HPP_47956DE583534FC082D245899751E369
#define UNGRD_CELL_POLICY_HPP_47956DE583534FC082D245899751E369

#include <array>

#include <cstddef>
#include <cstdint>

namespace ungrd {

template <typename TPositionIndex>
struct single_cell_policy {
  template <std::size_t N>
  using position = std::array<TPositionIndex, N>;

  template <std::size_t N>
  using location = position<N>;
};

using u32_single_cell_policy = single_cell_policy<std::uint32_t>;
using u64_single_cell_policy = single_cell_policy<std::uint64_t>;

} // namespace ungrd

#endif // UNGRD_CELL_POLICY_HPP_47956DE583534FC082D245899751E369
