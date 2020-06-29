#ifndef UNGRD_COMPACT_GRID_HPP_0420BAB69C7046B7AC6679E57C7A8D81
#define UNGRD_COMPACT_GRID_HPP_0420BAB69C7046B7AC6679E57C7A8D81

// Implements more or less the same algorithm as
//   https://github.com/InteractiveComputerGraphics/CompactNSearch
// by Jan Bender and other contributors.
//
// The main difference is that this is only the grid data structure and does not
// include a neighborhood search.

#include "cxx/assert.hpp"
#include "cxx/map.hpp"
#include "cxx/set.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <vector>

#include <cstddef>

#include <boost/range/adaptor/map.hpp>

namespace ungrd {

template <typename TEntry, size_t N>
class CompactGrid {
  static_assert(1 <= N and N <= 3);

public:
  using Entry = TEntry;
  using CellIndex = unsigned int;
  using GridPosition = std::array<int, N>;

private:
  template <typename Input>
  void LazyInit(Input const &input) {
    if (not init_) {
      size_t const entry_count = input.GetEntryCount();

      cells_.clear();
      map_.clear();
      old_pos_.resize(entry_count);
      new_pos_.resize(entry_count);

      std::vector<GridPosition> tmp_pos;

      for (Entry entry = 0; entry < entry_count; ++entry) {
        GridPosition const pos = input.GetEntryPosition(entry);
        old_pos_[entry] = new_pos_[entry] = pos;

        if (auto it = map_.find(pos); it != map_.end()) {
          auto &cell_data = cells_[it->second];
          cell_data.emplace_back(entry);
        } else {
          auto &cell_data = cells_.emplace_back();
          cell_data.reserve(50);
          cell_data.emplace_back(entry);
          tmp_pos.emplace_back(pos);
          map_[pos] = cells_.size() - 1;
        }
      }

      map_.clear();
      for (size_t cell_index = 0; cell_index < cells_.size(); ++cell_index) {
        map_.emplace(tmp_pos[cell_index], cell_index);
      }
    }

    init_ = true;
  }

public:
  template <typename Input>
  void Update(Input const &input) {
    LazyInit(input);

    size_t const entry_count = input.GetEntryCount();

    // update_point_sets

    std::swap(old_pos_, new_pos_);

    if (old_pos_.size() != entry_count) {
      old_pos_.resize(entry_count, invalid_pos);
      new_pos_.resize(entry_count);
    }

    for (Entry entry = 0; entry < entry_count; ++entry) {
      new_pos_[entry] = input.GetEntryPosition(entry);
    }

    // update_hash_table

    for (Entry entry = 0; entry < entry_count; ++entry) {
      if (new_pos_[entry] == old_pos_[entry])
        continue;

      auto const &pos = new_pos_[entry];

      if (auto it = map_.find(pos); it != map_.end()) {
        auto &cell_data = cells_[it->second];
        cell_data.push_back(entry);
      } else {
        auto &cell_data = cells_.emplace_back();
        cell_data.reserve(50);
        cell_data.emplace_back(entry);
        map_.emplace(pos, cells_.size() - 1);
      }

      {
        auto &cell_data = cells_[map_[old_pos_[entry]]];
        auto it = std::find(cell_data.begin(), cell_data.end(), entry);
        if (it != cell_data.end())
          cell_data.erase(it);
      }
    }
  }

  template <typename OutputIt>
  void CopyCellEntries(GridPosition const &pos, OutputIt output) const {
    if (auto it = map_.find(pos); it != map_.end()) {
      auto const cidx = it->second;
      auto const &cell_data = cells_[cidx];
      for (auto const entry : cell_data)
        *(output++) = entry;
    }
  }

public:
  auto KnownCells() const { return map_ | boost::adaptors::map_keys; }

public:
  CompactGrid() {
    cells_.emplace_back();
    map_[invalid_pos] = 0;
  }

  CompactGrid(CompactGrid const &) = delete;
  CompactGrid &operator=(CompactGrid const &) = delete;
  CompactGrid(CompactGrid &&) = default;
  CompactGrid &operator=(CompactGrid &&) = default;

private:
  bool init_ = false;

  hash_map<GridPosition, CellIndex> map_ = {};
  std::vector<std::vector<Entry>> cells_ = {};
  std::vector<GridPosition> new_pos_ = {};
  std::vector<GridPosition> old_pos_ = {};

  static constexpr GridPosition invalid_pos = {
      std::numeric_limits<int>::max(), std::numeric_limits<int>::max(),
      std::numeric_limits<int>::max()};
};

} // namespace ungrd

#endif // UNGRD_COMPACT_GRID_HPP_0420BAB69C7046B7AC6679E57C7A8D81
