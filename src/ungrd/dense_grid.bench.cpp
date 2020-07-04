#include <benchmark/benchmark.h>

#include "cxx/lexicographic_indexing.hpp"
#include "dense_grid.hpp"

#include <random>
#include <vector>

using namespace ungrd;

template <typename Grid>
auto &Grid_DenseCells_Input(benchmark::State &state) {
  constexpr size_t ndim = Grid::space_policy::ndim;
  using indexing_type = lexicographic_indexing<ndim>;

  std::array<size_t, ndim> extents;
  for (size_t dim = 0; dim < ndim; ++dim)
    extents[dim] = state.range(dim);

  indexing_type indexing{extents};

  auto const cpos_per_cell = state.range(ndim);

  using entry_type = typename Grid::entry_policy::entry;
  using position_type = typename Grid::space_policy::position;

  static std::vector<std::pair<entry_type, position_type>> input;
  input.clear();

  for (size_t cidx = 0; cidx < indexing.size(); ++cidx) {
    auto const nidx = indexing.decode(cidx);

    position_type cpos;
    for (size_t dim = 0; dim < ndim; ++dim)
      cpos[dim] = nidx[dim] - 4;

    for (size_t cpos_i = 0; cpos_i < cpos_per_cell; ++cpos_i)
      input.emplace_back(input.size(), cpos);
  }

  return input;
}

template <typename Grid>
auto &Grid_RandomCells_Input(benchmark::State &state) {
  std::mt19937 gen{0};
  std::uniform_int_distribution<size_t> dis;
  using dis_param = typename decltype(dis)::param_type;

  constexpr size_t ndim = Grid::space_policy::ndim;
  using indexing_type = lexicographic_indexing<ndim>;

  std::array<size_t, ndim> extents;
  for (size_t dim = 0; dim < ndim; ++dim)
    extents[dim] = state.range(dim);

  indexing_type indexing{extents};

  auto const cpos_count = state.range(ndim);

  using entry_type = typename Grid::entry_policy::entry;
  using position_type = typename Grid::space_policy::position;

  static std::vector<std::pair<entry_type, position_type>> input;
  input.clear();

  for (size_t cpos_i = 0; cpos_i < cpos_count; ++cpos_i) {
    auto const cidx = dis(gen, dis_param{0, indexing.size()});

    auto const nidx = indexing.decode(cidx);

    position_type cpos;
    for (size_t dim = 0; dim < ndim; ++dim)
      cpos[dim] = nidx[dim] - 4;

    input.emplace_back(input.size(), cpos);
  }

  return input;
}

template <typename Grid, typename Input>
void BMT_Grid_FirstUpdate(benchmark::State &state, Input const &input) {
  state.counters["ne"] = input.size();

  Grid grid;

  for (auto _ : state) {
    grid.update(input);
  }

  state.counters["nfc"] = grid.count_filled_cells();
}

template <typename Grid, typename Input>
void BMT_Grid_NoChangeUpdate(benchmark::State &state, Input const &input) {
  state.counters["ne"] = input.size();

  Grid grid;
  grid.update(input);

  for (auto _ : state) {
    grid.update(input);
  }

  state.counters["nfc"] = grid.count_filled_cells();
}

template <typename Grid, typename Input>
void BMT_Grid_CountAllEntries(benchmark::State &state, Input const &input) {
  state.counters["ne"] = input.size();

  Grid grid;
  grid.update(input);

  size_t count;
  for (auto _ : state) {
    count = 0;
    grid.foreach_position([&grid, &count](auto &cpos) {
      grid.foreach_entry_at_position(cpos, [&count](auto const) { ++count; });
    });
  }

  state.counters["nfc"] = grid.count_filled_cells();
  state.counters["nge"] = count;
}

#define EXTENT_RANGE                                                           \
  { 32, 32 }

#define DENSE_ENTRY_RANGE                                                      \
  { 1, 64 }

#define RANDOM_ENTRY_RANGE                                                     \
  { 32 * 32 * 32, 32 * 32 * 32 * 64 }

// FirstUpdate

template <typename Grid>
void BMT_Grid_FirstUpdate_DenseCells(benchmark::State &state) {
  auto const &input = Grid_DenseCells_Input<Grid>(state);
  BMT_Grid_FirstUpdate<Grid>(state, input);
}
BENCHMARK_TEMPLATE(BMT_Grid_FirstUpdate_DenseCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, DENSE_ENTRY_RANGE});

template <typename Grid>
void BMT_Grid_FirstUpdate_RandomCells(benchmark::State &state) {
  auto const &input = Grid_RandomCells_Input<Grid>(state);
  BMT_Grid_FirstUpdate<Grid>(state, input);
}
BENCHMARK_TEMPLATE(BMT_Grid_FirstUpdate_RandomCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, RANDOM_ENTRY_RANGE});

// NoChangeUpdate

template <typename Grid>
void BMT_Grid_NoChangeUpdate_DenseCells(benchmark::State &state) {
  auto const &input = Grid_DenseCells_Input<Grid>(state);
  BMT_Grid_NoChangeUpdate<Grid>(state, input);
}
BENCHMARK_TEMPLATE(BMT_Grid_NoChangeUpdate_DenseCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, DENSE_ENTRY_RANGE});

template <typename Grid>
void BMT_Grid_NoChangeUpdate_RandomCells(benchmark::State &state) {
  auto const &input = Grid_RandomCells_Input<Grid>(state);
  BMT_Grid_NoChangeUpdate<Grid>(state, input);
}
BENCHMARK_TEMPLATE(BMT_Grid_NoChangeUpdate_RandomCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, RANDOM_ENTRY_RANGE});

// CountAllEntries

template <typename Grid>
void BMT_Grid_CountAllEntries_DenseCells(benchmark::State &state) {
  auto const &input = Grid_DenseCells_Input<Grid>(state);
  BMT_Grid_CountAllEntries<Grid>(state, input);
}
BENCHMARK_TEMPLATE(BMT_Grid_CountAllEntries_DenseCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, DENSE_ENTRY_RANGE});

template <typename Grid>
void BMT_Grid_CountAllEntries_RandomCells(benchmark::State &state) {
  auto const &input = Grid_RandomCells_Input<Grid>(state);
  BMT_Grid_CountAllEntries<Grid>(state, input);
}
BENCHMARK_TEMPLATE(BMT_Grid_CountAllEntries_RandomCells, s32_e32_dense_grid<3>)
    ->Ranges({EXTENT_RANGE, EXTENT_RANGE, EXTENT_RANGE, RANDOM_ENTRY_RANGE});

#undef EXTENT_RANGE
