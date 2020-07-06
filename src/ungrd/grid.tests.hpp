#ifndef UNGRD_GRID_TESTS_HPP_FB7E119405514117B3B1C0B9C60C0F65
#define UNGRD_GRID_TESTS_HPP_FB7E119405514117B3B1C0B9C60C0F65

#include <gtest/gtest.h>

#include "cxx/set.hpp"

#include <vector>

namespace ungrd {

template <typename Grid>
void T_Grid_Correctness() {
  using grid_type = Grid;
  using position_type = typename grid_type::space_policy::position;
  using entry_type = typename grid_type::entry_policy::entry;

  grid_type grid;

  std::vector<std::pair<position_type, entry_type>> input = {
      {{-1, -2, -3}, 5}, {{3, 2, 1}, 2}, {{-1, -2, -3}, 1},
      {{3, 2, 1}, 3},    {{1, 1, 1}, 4}, {{1, 1, 2}, 0},
  };

  grid.update(input);

  {
    hash_set<entry_type> entries;
    auto const collect_entries = [&entries](auto const entry) {
      entries.insert(entry);
    };

    entries.clear();
    grid.foreach_entry_at_position({-999, -999, -999}, collect_entries);
    ASSERT_EQ(0, entries.size());

    entries.clear();
    grid.foreach_entry_at_position({-1, -2, -3}, collect_entries);
    ASSERT_EQ(2, entries.size());
    ASSERT_TRUE(entries.contains(5));
    ASSERT_TRUE(entries.contains(1));

    entries.clear();
    grid.foreach_entry_at_position({0, 0, 0}, collect_entries);
    ASSERT_EQ(0, entries.size());

    entries.clear();
    grid.foreach_entry_at_position({1, 1, 1}, collect_entries);
    ASSERT_EQ(1, entries.size());
    ASSERT_TRUE(entries.contains(4));

    entries.clear();
    grid.foreach_entry_at_position({1, 1, 2}, collect_entries);
    ASSERT_EQ(1, entries.size());
    ASSERT_TRUE(entries.contains(0));

    entries.clear();
    grid.foreach_entry_at_position({3, 2, 1}, collect_entries);
    ASSERT_EQ(2, entries.size());
    ASSERT_TRUE(entries.contains(2));
    ASSERT_TRUE(entries.contains(3));

    entries.clear();
    grid.foreach_entry_at_position({999, 999, 999}, collect_entries);
    ASSERT_EQ(0, entries.size());
  }

  {
    hash_set<position_type> positions;
    auto const collect_positions = [&positions](auto const &position) {
      positions.insert(position);
    };

    positions.clear();
    grid.foreach_position(collect_positions);
    ASSERT_EQ(4, positions.size());
    ASSERT_TRUE(positions.contains({-1, -2, -3}));
    ASSERT_TRUE(positions.contains({1, 1, 1}));
    ASSERT_TRUE(positions.contains({1, 1, 2}));
    ASSERT_TRUE(positions.contains({3, 2, 1}));
  }

  std::vector<std::pair<position_type, entry_type>> fresh = {
      {{-4, -5, -6}, 8},
      {{4, 3, 2}, 6},
      {{1, 1, 2}, 7},
  };

  std::vector<std::pair<position_type, entry_type>> stale = {
      {{3, 2, 1}, 3},
      {{1, 1, 2}, 0},
      {{-1, -2, -3}, 5},
      {{-1, -2, -3}, 1},
  };

  {
    grid.differential_update(fresh, stale);

    hash_set<entry_type> entries;
    auto const collect_entries = [&entries](auto const entry) {
      entries.insert(entry);
    };

    entries.clear();
    grid.foreach_entry_at_position({-999, -999, -999}, collect_entries);
    ASSERT_EQ(0, entries.size());

    entries.clear();
    grid.foreach_entry_at_position({-4, -5, -6}, collect_entries);
    ASSERT_EQ(1, entries.size());
    ASSERT_TRUE(entries.contains(8));

    entries.clear();
    grid.foreach_entry_at_position({-1, -2, -3}, collect_entries);
    ASSERT_EQ(0, entries.size());

    entries.clear();
    grid.foreach_entry_at_position({0, 0, 0}, collect_entries);
    ASSERT_EQ(0, entries.size());

    entries.clear();
    grid.foreach_entry_at_position({1, 1, 1}, collect_entries);
    ASSERT_EQ(1, entries.size());
    ASSERT_TRUE(entries.contains(4));

    entries.clear();
    grid.foreach_entry_at_position({1, 1, 2}, collect_entries);
    ASSERT_EQ(1, entries.size());
    ASSERT_TRUE(entries.contains(7));

    entries.clear();
    grid.foreach_entry_at_position({3, 2, 1}, collect_entries);
    ASSERT_EQ(1, entries.size());
    ASSERT_TRUE(entries.contains(2));

    entries.clear();
    grid.foreach_entry_at_position({4, 3, 2}, collect_entries);
    ASSERT_EQ(1, entries.size());
    ASSERT_TRUE(entries.contains(6));

    entries.clear();
    grid.foreach_entry_at_position({999, 999, 999}, collect_entries);
    ASSERT_EQ(0, entries.size());
  }
}

} // namespace ungrd

#endif // UNGRD_GRID_TESTS_HPP_FB7E119405514117B3B1C0B9C60C0F65
