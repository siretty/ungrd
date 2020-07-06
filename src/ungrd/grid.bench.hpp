#ifndef UNGRD_GRID_BENCH_HPP_A7C1454A8E884C0BBBF465165423A19B
#define UNGRD_GRID_BENCH_HPP_A7C1454A8E884C0BBBF465165423A19B

#include "cxx/lexicographic_indexing.hpp"

#include <random>
#include <vector>

namespace ungrd {

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
void BMT_Grid_AllMoveOneUpdate(benchmark::State &state, Input &input) {
  state.counters["ne"] = input.size();

  Grid grid;
  grid.update(input);

  constexpr size_t ndim = Grid::space_policy::ndim;

  for (auto _ : state) {
    for (auto &[entry, cpos] : input)
      for (size_t dim = 0; dim < ndim; ++dim)
        ++cpos[dim];

    grid.update(input);
  }

  state.counters["nfc"] = grid.count_filled_cells();
}

template <typename Grid, typename Input>
void BMT_Grid_SomeMoveOneUpdate(benchmark::State &state, Input &input) {
  state.counters["ne"] = input.size();

  std::mt19937 gen{1};
  std::uniform_real_distribution<double> dis{0., 1.};

  Grid grid;
  grid.update(input);

  constexpr size_t ndim = Grid::space_policy::ndim;

  auto const move_count = state.range(ndim + 1);
  double move_prob = static_cast<double>(input.size()) / move_count;

  for (auto _ : state) {
    for (auto &[entry, cpos] : input) {
      if (dis(gen) <= move_prob)
        for (size_t dim = 0; dim < ndim; ++dim)
          ++cpos[dim];
    }

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

// FirstUpdate

template <typename Grid>
void BMT_Grid_FirstUpdate_DenseCells(benchmark::State &state) {
  auto const &input = Grid_DenseCells_Input<Grid>(state);
  BMT_Grid_FirstUpdate<Grid>(state, input);
}

template <typename Grid>
void BMT_Grid_FirstUpdate_RandomCells(benchmark::State &state) {
  auto const &input = Grid_RandomCells_Input<Grid>(state);
  BMT_Grid_FirstUpdate<Grid>(state, input);
}

// NoChangeUpdate

template <typename Grid>
void BMT_Grid_NoChangeUpdate_DenseCells(benchmark::State &state) {
  auto const &input = Grid_DenseCells_Input<Grid>(state);
  BMT_Grid_NoChangeUpdate<Grid>(state, input);
}

template <typename Grid>
void BMT_Grid_NoChangeUpdate_RandomCells(benchmark::State &state) {
  auto const &input = Grid_RandomCells_Input<Grid>(state);
  BMT_Grid_NoChangeUpdate<Grid>(state, input);
}

// AllMoveOneUpdate

template <typename Grid>
void BMT_Grid_AllMoveOneUpdate_DenseCells(benchmark::State &state) {
  auto &input = Grid_DenseCells_Input<Grid>(state);
  BMT_Grid_AllMoveOneUpdate<Grid>(state, input);
}

template <typename Grid>
void BMT_Grid_AllMoveOneUpdate_RandomCells(benchmark::State &state) {
  auto &input = Grid_RandomCells_Input<Grid>(state);
  BMT_Grid_AllMoveOneUpdate<Grid>(state, input);
}

// SomeMoveOneUpdate

template <typename Grid>
void BMT_Grid_SomeMoveOneUpdate_DenseCells(benchmark::State &state) {
  auto &input = Grid_DenseCells_Input<Grid>(state);
  BMT_Grid_SomeMoveOneUpdate<Grid>(state, input);
}

template <typename Grid>
void BMT_Grid_SomeMoveOneUpdate_RandomCells(benchmark::State &state) {
  auto &input = Grid_RandomCells_Input<Grid>(state);
  BMT_Grid_SomeMoveOneUpdate<Grid>(state, input);
}

// CountAllEntries

template <typename Grid>
void BMT_Grid_CountAllEntries_DenseCells(benchmark::State &state) {
  auto const &input = Grid_DenseCells_Input<Grid>(state);
  BMT_Grid_CountAllEntries<Grid>(state, input);
}

template <typename Grid>
void BMT_Grid_CountAllEntries_RandomCells(benchmark::State &state) {
  auto const &input = Grid_RandomCells_Input<Grid>(state);
  BMT_Grid_CountAllEntries<Grid>(state, input);
}

#define EXTENT_RANGE                                                           \
  { 32, 32 }

#define DENSE_ENTRY_RANGE                                                      \
  { 1, 64 }

#define RANDOM_ENTRY_RANGE                                                     \
  { 32 * 32 * 32, 32 * 32 * 32 * 64 }

} // namespace ungrd

#endif // UNGRD_GRID_BENCH_HPP_A7C1454A8E884C0BBBF465165423A19B
