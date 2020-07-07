#include "static_bitset.hpp"

#include <gtest/gtest.h>

TEST(StaticBitset, Correctness) {
  ASSERT_LE(8, sizeof(ungrd::static_bitset<64>));
  ASSERT_LT(8, sizeof(ungrd::static_bitset<65>));
  ASSERT_LE(16, sizeof(ungrd::static_bitset<128>));
  ASSERT_LT(16, sizeof(ungrd::static_bitset<129>));

  ungrd::static_bitset<65, unsigned> bits;
  ASSERT_EQ(65, bits.size());

  bits.set_all();
  ASSERT_EQ(65, bits.count_set_bits());
  ASSERT_TRUE(bits.all());
  ASSERT_TRUE(bits.any());
  ASSERT_FALSE(bits.none());
  for (size_t index = 0; index < bits.size(); ++index)
    ASSERT_TRUE(bits.get(index));

  bits.reset_all();
  ASSERT_FALSE(bits.all());
  ASSERT_FALSE(bits.any());
  ASSERT_TRUE(bits.none());
  for (size_t index = 0; index < bits.size(); ++index)
    ASSERT_FALSE(bits.get(index));

  bits.reset_all();

  bits.set(7);
  ASSERT_TRUE(bits.get(7));
  ASSERT_FALSE(bits.all());
  ASSERT_TRUE(bits.any());
  ASSERT_FALSE(bits.none());

  bits.set(37);
  ASSERT_TRUE(bits.get(37));
  ASSERT_FALSE(bits.all());
  ASSERT_TRUE(bits.any());
  ASSERT_FALSE(bits.none());

  for (size_t index = 0; index < bits.size(); ++index) {
    switch (index) {
    case 7:
    case 37:
      ASSERT_TRUE(bits.get(index));
      break;
    default:
      ASSERT_FALSE(bits.get(index));
      break;
    }
  }

  bits.reset_all();
  ASSERT_EQ(0, bits.count_set_bits());

  bits.set(64);
  ASSERT_FALSE(bits.all());
  ASSERT_TRUE(bits.any());
  ASSERT_FALSE(bits.none());
  ASSERT_EQ(64, bits.countr_zero());
  ASSERT_EQ(0, bits.countr_one());
  ASSERT_EQ(1, bits.count_set_bits());

  bits.set(3);
  ASSERT_EQ(3, bits.countr_zero());
  ASSERT_EQ(0, bits.countr_one());
  ASSERT_EQ(2, bits.count_set_bits());

  bits.set(2);
  bits.set(1);
  bits.set(0);
  ASSERT_EQ(0, bits.countr_zero());
  ASSERT_EQ(4, bits.countr_one());
  ASSERT_EQ(5, bits.count_set_bits());
}
