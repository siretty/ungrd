#include <gtest/gtest.h>

#include "compact_grid.hpp"
#include "grid.tests.hpp"

using namespace ungrd;

TEST(CompactGrid, Correctness) {
  T_Grid_Correctness<s32_e32_compact_grid<3>>();
}
