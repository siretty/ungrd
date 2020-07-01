#ifndef UNGRD_MAP_HPP_1E565FBF980E46BBA7007A105053BD2E
#define UNGRD_MAP_HPP_1E565FBF980E46BBA7007A105053BD2E

#include "phmap/phmap.h"

namespace ungrd {

template <typename K, typename V, typename H>
using hash_map = phmap::flat_hash_map<K, V, H>;

} // namespace ungrd

#endif // UNGRD_MAP_HPP_1E565FBF980E46BBA7007A105053BD2E
