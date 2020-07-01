#include "uniform_grid.hpp"

#include "compact_grid.hpp"
#include "compact_multi_grid.hpp"

namespace ungrd {

UniformGrid::UniformGrid() {
  CompactGrid<int, 3> c_i3;
  CompactMultiGrid<int, 3> cm_i3;
}

} // namespace ungrd
