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
#include "entry_policy.hpp"
#include "object_pool.hpp"
#include "space_policy.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <vector>

#include <cstddef>

#include <boost/container_hash/hash.hpp>
#include <boost/range/adaptor/map.hpp>

namespace ungrd {

template <typename PSpace, typename PEntry>
class compact_grid {
public:
  using space_policy = PSpace;
  using entry_policy = PEntry;

private:
  static constexpr std::size_t ndim = space_policy::ndim;
  using position_type = typename space_policy::position;
  using position_hash = boost::hash<position_type>;

  using entry_type = typename entry_policy::entry;

  using entry_vector = std::vector<entry_type>;

private:
  using cidx_type = std::size_t;

  class cell_type {
  public:
    bool empty() const { return entries_.empty(); }

    auto const &entries() const { return entries_; }

    void reserve_entries(size_t count) { entries_.reserve(count); }

    void add_entry(entry_type entry) {
      using std::begin, std::end;
      auto it = std::find(begin(entries_), end(entries_), entry);
      if (it == end(entries_))
        entries_.emplace_back(entry);
    }

    void erase_entry(entry_type entry) {
      using std::begin, std::end;
      auto it = std::find(begin(entries_), end(entries_), entry);
      if (it != end(entries_))
        entries_.erase(it);
    }

    friend void swap(cell_type &a, cell_type &b) {
      using std::swap;
      swap(a.entries_, b.entries_);
    }

  public:
    cell_type() = default;

  private:
    entry_vector entries_ = {};
  };

public:
  size_t count_filled_cells() const {
    size_t count = 0;
    for (auto const &cell : cells_)
      count += not cell.empty();
    return count;
  }

public:
  template <typename FCallback>
  void foreach_entry_at_position(
      position_type const &cpos, FCallback callback) const {
    if (auto it = map_.find(cpos); it != map_.end())
      for (auto const entry : cells_[it->second].entries())
        callback(entry);
  }

  template <typename FCallback>
  void foreach_position(FCallback callback) const {
    for (auto const &[cpos, cidx] : map_) {
      auto const &cell = cells_[cidx];
      if (not cell.empty())
        callback(cpos);
    }
  }

public:
  template <typename TInput>
  void update(TInput const &input) {
    using std::size;
    size_t const entry_count = size(input);

    cells_.clear();
    map_.clear();

    std::array<std::pair<position_type, entry_type>, 0> dummy_stale;
    differential_update(input, dummy_stale);

    // think about calling map_.rehash(0);
  }

public:
  template <typename TFresh, typename TStale>
  void differential_update(TFresh const &fresh, TStale const &stale) {
    for (auto const &[cpos, entry] : fresh) {
      if (auto it = map_.find(cpos); it != map_.end()) {
        auto &cell = cells_[it->second];
        cell.add_entry(entry);
      } else {
        auto &cell = cells_.emplace_back();
        cell.reserve_entries(50);
        cell.add_entry(entry);
        map_[cpos] = cells_.size() - 1;
      }
    }

    for (auto const &[cpos, entry] : stale) {
      if (auto it = map_.find(cpos); it != map_.end())
        cells_[it->second].erase_entry(entry);
    }
  }

public:
  compact_grid() {
    cells_.emplace_back();
    map_[invalid_pos] = 0;
  }

  compact_grid(compact_grid const &) = delete;
  compact_grid &operator=(compact_grid const &) = delete;
  compact_grid(compact_grid &&) = default;
  compact_grid &operator=(compact_grid &&) = default;

private:
  bool init_ = false;

  hash_map<position_type, cidx_type, position_hash> map_ = {};
  std::vector<cell_type> cells_ = {};

  static constexpr position_type invalid_pos =
      space_policy::most_positive_position();
};

template <size_t NDim>
using s32_e32_compact_grid =
    compact_grid<s32_space_policy<NDim>, u32_entry_policy>;

} // namespace ungrd

#endif // UNGRD_COMPACT_GRID_HPP_0420BAB69C7046B7AC6679E57C7A8D81
