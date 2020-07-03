#include <gtest/gtest.h>

#include "modulo.hpp"

TEST(Modulo, Correctness) {
  using namespace ungrd;

  ASSERT_EQ(4, modulo(-11, 5));
  ASSERT_EQ(4, modulo(-6, 5));
  ASSERT_EQ(4, modulo(-1, 5));
  ASSERT_EQ(4, modulo(4, 5));
  ASSERT_EQ(4, modulo(9, 5));

  ASSERT_EQ(0, modulo(-10, 5));
  ASSERT_EQ(0, modulo(-5, 5));
  ASSERT_EQ(0, modulo(0, 5));
  ASSERT_EQ(0, modulo(5, 5));
  ASSERT_EQ(0, modulo(10, 5));
}
