#include <benchmark/benchmark.h>

#include "dense_grid.hpp"
#include "grid.bench.hpp"

using namespace ungrd;

// FirstUpdate

BENCHMARK_TEMPLATE(BMT_Grid_FirstUpdate_DenseCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, DENSE_ENTRY_RANGE});

BENCHMARK_TEMPLATE(BMT_Grid_FirstUpdate_RandomCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, RANDOM_ENTRY_RANGE});

// NoChangeUpdate

BENCHMARK_TEMPLATE(BMT_Grid_NoChangeUpdate_DenseCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, DENSE_ENTRY_RANGE});

BENCHMARK_TEMPLATE(BMT_Grid_NoChangeUpdate_RandomCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, RANDOM_ENTRY_RANGE});

// AllMoveOneUpdate

BENCHMARK_TEMPLATE(BMT_Grid_AllMoveOneUpdate_DenseCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, DENSE_ENTRY_RANGE});

BENCHMARK_TEMPLATE(BMT_Grid_AllMoveOneUpdate_RandomCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, RANDOM_ENTRY_RANGE});

// SomeMoveOneUpdate

BENCHMARK_TEMPLATE(BMT_Grid_SomeMoveOneUpdate_RandomCells, s32_e32_dense_grid<3>)
    ->Ranges(
        {EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, RANDOM_ENTRY_RANGE,
         RANDOM_ENTRY_RANGE});

// CountAllEntries

BENCHMARK_TEMPLATE(BMT_Grid_CountAllEntries_DenseCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, DENSE_ENTRY_RANGE});

BENCHMARK_TEMPLATE(BMT_Grid_CountAllEntries_RandomCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, RANDOM_ENTRY_RANGE});
