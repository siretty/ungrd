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
      {5, {-1, -2, -3}}, {2, {3, 2, 1}}, {1, {-1, -2, -3}},
      {3, {3, 2, 1}},    {4, {1, 1, 1}}, {0, {1, 1, 2}},
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
}

} // namespace ungrd

#endif // UNGRD_GRID_TESTS_HPP_FB7E119405514117B3B1C0B9C60C0F65
