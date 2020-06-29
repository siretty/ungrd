#ifndef UNGRD_COMPACT_MULTI_GRID_HPP_6992AD06F6E24BB19D75D9DBD5545A26
#define UNGRD_COMPACT_MULTI_GRID_HPP_6992AD06F6E24BB19D75D9DBD5545A26

#include "cxx/assert.hpp"
#include "cxx/map.hpp"
#include "cxx/set.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <vector>

#include <cstddef>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/set_algorithm.hpp>

namespace ungrd {

template <typename TEntry, size_t N>
class CompactMultiGrid {
  static_assert(1 <= N and N <= 3);

public:
  using Entry = TEntry;
  using CellIndex = unsigned int;
  using GridPosition = std::array<int, N>;

private:
  struct LexicographicalOrder {
    bool operator()(GridPosition const &lhs, GridPosition const &rhs) const {
      return std::lexicographical_compare(
          lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
  };

private:
  struct {
    small_sort_set<GridPosition, LexicographicalOrder, 1> new_entry_positions;
    small_sort_set<GridPosition, LexicographicalOrder, 1> old_entry_positions;

    std::vector<GridPosition> fresh_positions;
    std::vector<GridPosition> stale_positions;
  } update_;

public:
  template <typename Input>
  void Update(Input const &input) {
    size_t const entry_count = input.GetEntryCount();

    entries_.resize(entry_count);

    auto &new_entry_positions = update_.new_entry_positions;
    auto &old_entry_positions = update_.old_entry_positions;

    auto &fresh_positions = update_.fresh_positions;
    auto &stale_positions = update_.stale_positions;

    for (Entry entry = 0; entry < entry_count; ++entry) {
      // retrieve the current grid positions of entry
      new_entry_positions.clear();
      input.ForeachEntryPosition(
          entry, [&s = new_entry_positions](GridPosition const &pos) {
            s.emplace(pos);
          });

      auto &entry_data = entries_[entry];

      // resolve previous grid positions of entry
      old_entry_positions.clear();
      for (auto const cidx : entry_data.GetCellIndices()) {
        auto const &cell_data = cells_[cidx];
        old_entry_positions.emplace(cell_data.GetGridPosition());
      }

      if (new_entry_positions == old_entry_positions)
        continue;

      // compute the fresh grid positions (ie. entered)
      fresh_positions.clear();
      boost::set_difference(
          new_entry_positions, old_entry_positions,
          std::back_inserter(fresh_positions));

      for (auto const &pos : fresh_positions) {
        CellIndex cidx;
        if (auto it = map_.find(pos); it != map_.end()) {
          // fetch exisiting cell
          cidx = it->second;
        } else {
          // create new cell
          cidx = cells_.size();
          cells_.emplace_back(pos);
          map_.emplace(pos, cidx);
        }
        // connect entry and cell
        auto &cell_data = cells_[cidx];
        cell_data.AddEntry(entry);
        entry_data.AddCellIndex(cidx);
      }

      // compute the stale grid positions (ie. exited)
      stale_positions.clear();
      boost::set_difference(
          old_entry_positions, new_entry_positions,
          std::back_inserter(stale_positions));

      for (auto const &pos : stale_positions) {
        CellIndex cidx;
        if (auto it = map_.find(pos); it != map_.end()) {
          cidx = it->second;
        } else {
          // this is not supposed to happen
          throw "not supposed to happen";
        }
        // disconnect entry and cell
        auto &cell_data = cells_[cidx];
        cell_data.RemoveEntry(entry);
        entry_data.RemoveCellIndex(cidx);
      }
    }
  }

  template <typename OutputIt>
  void CopyCellEntries(GridPosition const &pos, OutputIt output) const {
    if (auto it = map_.find(pos); it != map_.end()) {
      auto const cidx = it->second;
      auto const &cell_data = cells_[cidx];
      for (auto const entry : cell_data.GetEntries())
        *(output++) = entry;
    }
  }

public:
  auto KnownCells() const {
    return cells_ | boost::adaptors::transformed([](auto const &cell_data) {
             return cell_data.GetGridPosition();
           });
  }

private:
  class CellData {
  public:
    CellData(GridPosition const &position) : position_{position} {
      entries_.reserve(50);
    }

  public:
    void AddEntry(Entry const entry) { entries_.emplace_back(entry); }

    void RemoveEntry(Entry const entry) {
      auto it = std::find(entries_.begin(), entries_.end(), entry);
      if (it != entries_.end())
        entries_.erase(it);
    }

    GridPosition const &GetGridPosition() const { return position_; }

    auto const &GetEntries() const { return entries_; }

  private:
    GridPosition position_;
    std::vector<Entry> entries_;
  };

  class EntryData {
  public:
    void Reset() { cell_indices_.clear(); }

    void AddCellIndex(CellIndex const cidx) { cell_indices_.emplace(cidx); }

    void RemoveCellIndex(CellIndex const cidx) { cell_indices_.erase(cidx); }

    auto const &GetCellIndices() const { return cell_indices_; }

  public:
    EntryData() {}

    friend void swap(EntryData &a, EntryData &b) {
      using std::swap;
      swap(a.cell_indices_, b.cell_indices_);
    }

  private:
    small_sort_set<CellIndex, std::less<CellIndex>, 1> cell_indices_;
  };

public:
  CompactMultiGrid() = default;

  CompactMultiGrid(CompactMultiGrid const &) = delete;
  CompactMultiGrid &operator=(CompactMultiGrid const &) = delete;
  CompactMultiGrid(CompactMultiGrid &&) = default;
  CompactMultiGrid &operator=(CompactMultiGrid &&) = default;

private:
  hash_map<GridPosition, CellIndex> map_ = {};
  std::vector<CellData> cells_ = {};
  std::vector<EntryData> entries_ = {};
};

} // namespace ungrd

#endif // UNGRD_COMPACT_MULTI_GRID_HPP_6992AD06F6E24BB19D75D9DBD5545A26
