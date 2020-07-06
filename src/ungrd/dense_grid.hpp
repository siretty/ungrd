#ifndef UNGRD_DENSE_GRID_HPP_68F5A7B39EA04EF4A38B5422B2E0A742
#define UNGRD_DENSE_GRID_HPP_68F5A7B39EA04EF4A38B5422B2E0A742

#include "cxx/lexicographic_indexing.hpp"
#include "cxx/map.hpp"

#include "entry_policy.hpp"
#include "object_pool.hpp"
#include "space_policy.hpp"

#include <algorithm>
#include <array>
#include <optional>
#include <utility>
#include <vector>

#include <boost/container_hash/hash.hpp>

namespace ungrd {

template <typename PSpace, typename PEntry>
class dense_grid {
public:
  using space_policy = PSpace;
  using entry_policy = PEntry;

private:
  static constexpr std::size_t ndim = space_policy::ndim;
  using position_type = typename space_policy::position;
  using position_hash = boost::hash<position_type>;

  using entry_type = typename entry_policy::entry;

  using indexing_type = lexicographic_indexing<ndim>;
  using ndidx_type = typename indexing_type::ndidx_type;

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
    for (auto const &cell : cidx_to_cell_) {
      count += not cell.empty();
    }
    return count;
  }

public:
  template <typename FCallback>
  void foreach_entry_at_position(
      position_type const &cpos, FCallback callback) const {
    if (auto const ndidx = try_cpos_to_ndidx(cpos, offsets_, indexing_)) {
      auto const cidx = indexing_.encode(*ndidx);
      auto const &cell = cidx_to_cell_[cidx];
      if (not cell.empty())
        for (auto const entry : cidx_to_cell_[cidx].entries())
          callback(entry);
    }
  }

  template <typename FCallback>
  void foreach_position(FCallback callback) const {
    for (size_t cidx = 0; cidx < cidx_to_cell_.size(); ++cidx) {
      if (not cidx_to_cell_[cidx].empty()) {
        auto const ndidx = indexing_.decode(cidx);
        auto const cpos = ndidx_to_cpos(ndidx);

        callback(cpos);
      }
    }
  }

public:
  template <typename TInput>
  void update(TInput const &input) {
    position_type lo = space_policy::most_positive_position();
    position_type hi = space_policy::most_negative_position();

    auto &map = update_map_;
    map.clear();

    for (auto const [cpos, entry] : input) {
      auto [it, inserted] = map.try_emplace(cpos);

      auto &cell = it->second;
      if (inserted)
        cell.reserve_entries(50);

      cell.add_entry(entry);

      // update lo and hi cell positions
      for (size_t dim = 0; dim < ndim; ++dim) {
        lo[dim] = std::min(lo[dim], cpos[dim]);
        hi[dim] = std::max(hi[dim], cpos[dim]);
      }
    }

    if (map.size() == 0) {
      offsets_.fill(0);
    } else {
      position_type offsets;
      ndidx_type extents;
      for (size_t dim = 0; dim < ndim; ++dim) {
        offsets[dim] = -lo[dim];
        extents[dim] = hi[dim] - lo[dim] + 1;
      }

      reshape(offsets, extents);

      for (auto &[cpos, cell] : map) {
        auto const ndidx = cpos_to_ndidx(cpos);
        auto const cidx = indexing_.encode(ndidx);

        using std::swap;
        swap(cidx_to_cell_[cidx], cell);
      }
    }
  }

public:
  template <typename TFresh, typename TStale>
  void differential_update(TFresh const &fresh, TStale const &stale) {
    // remove stale entries from their cells
    for (auto const &[cpos, entry] : stale) {
      auto const ndidx = cpos_to_ndidx(cpos);
      auto const cidx = indexing_.encode(ndidx);

      cidx_to_cell_[cidx].erase_entry(entry);
    }

    // initialize lo and hi cell positions
    position_type lo = offsets_;
    position_type hi;
    for (size_t dim = 0; dim < ndim; ++dim) {
      hi[dim] = offsets_[dim] + indexing_.extent(dim);
    }

    auto &map = update_map_;
    map.clear();

    for (auto const &[cpos, entry] : fresh) {
      auto const ndidx = cpos_to_ndidx(cpos);

      if (auto const cidx = indexing_.try_encode(ndidx)) {
        // known cell, just add the entry
        cidx_to_cell_[*cidx].add_entry(entry);
      } else {
        // try to add a new cell to the temporary map
        auto [it, inserted] = map.try_emplace(cpos);

        auto &cell = it->second;
        if (inserted)
          cell.reserve_entries(50);

        cell.add_entry(entry);

        // update lo and hi cell positions
        for (size_t dim = 0; dim < ndim; ++dim) {
          lo[dim] = std::min(lo[dim], cpos[dim]);
          hi[dim] = std::max(hi[dim], cpos[dim]);
        }
      }
    }

    if (map.size() > 0) {
      { // compute the new offsets and extents and reshape the array
        position_type new_offsets;
        ndidx_type new_extents;
        for (size_t dim = 0; dim < ndim; ++dim) {
          new_offsets[dim] = -lo[dim];
          new_extents[dim] = hi[dim] - lo[dim] + 1;
        }

        reshape(new_offsets, new_extents);
      }

      // consume the new cells
      for (auto &[cpos, cell] : map) {
        auto const ndidx = cpos_to_ndidx(cpos);
        auto const cidx = indexing_.encode(ndidx);

        using std::swap;
        swap(cidx_to_cell_[cidx], cell);
      }
    }
  }

private:
  void
  reshape(position_type const &new_offsets, ndidx_type const &new_extents) {
    using std::swap;

    auto const &old_indexing = indexing_;
    indexing_type new_indexing{new_extents};

    std::vector<cell_type> new_cidx_to_cell;
    new_cidx_to_cell.resize(new_indexing.size());

    for (size_t old_cidx = 0; old_cidx < old_indexing.size(); ++old_cidx) {
      auto const old_ndidx = old_indexing.decode(old_cidx);
      auto const cpos = ndidx_to_cpos(old_ndidx);

      if (auto new_ndidx = try_cpos_to_ndidx(cpos, new_offsets, new_indexing)) {
        auto const new_cidx = new_indexing.encode(*new_ndidx);

        swap(new_cidx_to_cell[new_cidx], cidx_to_cell_[old_cidx]);
      }
    }

    offsets_ = new_offsets;
    indexing_ = new_indexing;
    swap(cidx_to_cell_, new_cidx_to_cell);
  }

private:
  static constexpr std::optional<ndidx_type> try_cpos_to_ndidx(
      position_type const &cpos, position_type const &offsets,
      indexing_type const &indexing) {
    ndidx_type ndidx;

    for (size_t dim = 0; dim < ndim; ++dim) {
      auto const index = cpos[dim] + offsets[dim];
      if (index < 0 or index >= indexing.extent(dim))
        return std::nullopt;
      else
        ndidx[dim] = index;
    }

    return ndidx;
  }

  static constexpr auto
  cpos_to_ndidx(position_type const &cpos, position_type const &offsets) {
    ndidx_type ndidx;
    for (size_t dim = 0; dim < ndim; ++dim)
      ndidx[dim] = cpos[dim] + offsets[dim];
    return ndidx;
  }

  auto cpos_to_ndidx(position_type const &cpos) const {
    return cpos_to_ndidx(cpos, offsets_);
  }

  static constexpr auto
  ndidx_to_cpos(ndidx_type const &ndidx, position_type const &offsets) {
    position_type cpos;
    for (size_t dim = 0; dim < ndim; ++dim)
      cpos[dim] = ndidx[dim] - offsets[dim];
    return cpos;
  }

  auto ndidx_to_cpos(ndidx_type const &ndidx) const {
    return ndidx_to_cpos(ndidx, offsets_);
  }

public:
  dense_grid() = default;
  dense_grid(dense_grid const &) = delete;
  dense_grid &operator=(dense_grid const &) = delete;
  dense_grid(dense_grid &&) = default;
  dense_grid &operator=(dense_grid &&) = default;

private:
  position_type offsets_;
  indexing_type indexing_;

  std::vector<cell_type> cidx_to_cell_;
  hash_map<position_type, cell_type, position_hash> update_map_;
};

template <size_t NDim>
using s32_e32_dense_grid = dense_grid<s32_space_policy<NDim>, u32_entry_policy>;

} // namespace ungrd

#endif // UNGRD_DENSE_GRID_HPP_68F5A7B39EA04EF4A38B5422B2E0A742
