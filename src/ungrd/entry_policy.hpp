#ifndef UNGRD_ENTRY_POLICY_HPP_583302C5034A450E8E02ACB89FE58F51
#define UNGRD_ENTRY_POLICY_HPP_583302C5034A450E8E02ACB89FE58F51

#include <cstdint>

namespace ungrd {

template <typename TEntry>
struct entry_policy {
  using entry = TEntry;
};

using u32_entry_policy = entry_policy<std::uint32_t>;
using u64_entry_policy = entry_policy<std::uint64_t>;

} // namespace ungrd

#endif // UNGRD_ENTRY_POLICY_HPP_583302C5034A450E8E02ACB89FE58F51
