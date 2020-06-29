#ifndef UNGRD_MAP_HPP_1E565FBF980E46BBA7007A105053BD2E
#define UNGRD_MAP_HPP_1E565FBF980E46BBA7007A105053BD2E

#include <absl/container/flat_hash_map.h>

namespace ungrd {

template <typename K, typename V>
using hash_map = absl::flat_hash_map<K, V>;

} // namespace ungrd

#endif // UNGRD_MAP_HPP_1E565FBF980E46BBA7007A105053BD2E
