#include <gtest/gtest.h>

#include "dense_grid.hpp"
#include "grid.tests.hpp"

using namespace ungrd;

TEST(DenseGrid, Correctness) { T_Grid_Correctness<s32_e32_dense_grid<3>>(); }
