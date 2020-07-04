#ifndef UNGRD_DENSE_GRID_HPP_68F5A7B39EA04EF4A38B5422B2E0A742
#define UNGRD_DENSE_GRID_HPP_68F5A7B39EA04EF4A38B5422B2E0A742

#include "cxx/lexicographic_indexing.hpp"
#include "cxx/map.hpp"

#include "entry_policy.hpp"
#include "object_pool.hpp"
#include "space_policy.hpp"

#include <algorithm>
#include <array>
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

  using entry_vector = std::vector<entry_type>;
  using entry_vector_pool = object_pool<entry_vector>;
  // using entry_vector_pool = new_delete_object_pool<entry_vector>;

private:
  using cidx_type = std::size_t;

  class cell_type {
    using entry_vector_ptr = typename entry_vector_pool::acquire_unique_ptr;

  public:
    bool empty() const { return (not entries_) or entries_->empty(); }

    auto const &entries() const { return *entries_; }

    void add_entry(entry_type entry) {
      using std::begin, std::end;
      auto it = std::find(begin(*entries_), end(*entries_), entry);
      if (it == end(*entries_))
        entries_->emplace_back(entry);
    }

    friend void swap(cell_type &a, cell_type &b) {
      using std::swap;
      swap(a.entries_, b.entries_);
    }

  public:
    cell_type() = default;

    cell_type(entry_vector_pool &pool) : entries_{pool.acquire_unique()} {
      entries_->clear();
    }

  private:
    entry_vector_ptr entries_ = {};
  };

public:
  size_t count_filled_cells() const {
    size_t count = 0;
    foreach_position([&count](auto &cell) { count += not cell.empty(); });
    return count;
  }

public:
  template <typename FCallback>
  void foreach_entry_at_position(
      position_type const &cpos, FCallback callback) const {
    std::array<size_t, ndim> indices;

    for (size_t dim = 0; dim < ndim; ++dim) {
      auto const index = cpos[dim] + offsets_[dim];
      if (index < 0 or index >= indexing_.extent(dim))
        return;
      else
        indices[dim] = index;
    }

    auto const cidx = indexing_.encode(indices);
    auto const &cell = cidx_to_cell_[cidx];
    if (not cell.empty())
      for (auto const entry : cidx_to_cell_[cidx].entries())
        callback(entry);
  }

  template <typename FCallback>
  void foreach_position(FCallback callback) const {
    for (size_t cidx = 0; cidx < cidx_to_cell_.size(); ++cidx) {
      if (not cidx_to_cell_[cidx].empty()) {
        auto const indices = indexing_.decode(cidx);

        position_type cpos;
        for (size_t dim = 0; dim < ndim; ++dim)
          cpos[dim] = indices[dim] - offsets_[dim];

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

    for (auto const [entry, cpos] : input) {
      auto [it, _] = map.try_emplace(cpos, entry_vector_pool_);
      auto &cell = it->second;
      cell.add_entry(entry);
      for (size_t dim = 0; dim < ndim; ++dim) {
        lo[dim] = std::min(lo[dim], cpos[dim]);
        hi[dim] = std::max(hi[dim], cpos[dim]);
      }
    }

    if (map.size() == 0) {
      offsets_.fill(0);
    } else {
      std::array<size_t, ndim> extents;
      for (size_t dim = 0; dim < ndim; ++dim) {
        offsets_[dim] = -lo[dim];
        extents[dim] = hi[dim] - lo[dim] + 1;
      }

      indexing_ = indexing_type{extents};

      cidx_to_cell_.resize(indexing_.size());

      for (auto &[cpos, cell] : map) {
        std::array<size_t, ndim> indices;
        for (size_t dim = 0; dim < ndim; ++dim)
          indices[dim] = cpos[dim] + offsets_[dim];

        auto const cidx = indexing_.encode(indices);

        using std::swap;
        swap(cidx_to_cell_[cidx], cell);
      }
    }
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

  // must be dtored after cidx_to_cell_ and update_map_
  entry_vector_pool entry_vector_pool_;

  std::vector<cell_type> cidx_to_cell_;
  hash_map<position_type, cell_type, position_hash> update_map_;
};

template <size_t NDim>
using s32_e32_dense_grid = dense_grid<s32_space_policy<NDim>, u32_entry_policy>;

} // namespace ungrd

#endif // UNGRD_DENSE_GRID_HPP_68F5A7B39EA04EF4A38B5422B2E0A742
