#ifndef UNGRD_MODULO_HPP_120D8B06A5034DE596B874E50925595E
#define UNGRD_MODULO_HPP_120D8B06A5034DE596B874E50925595E

#include <type_traits>

#include <cassert>

namespace ungrd {

template <typename LHS, typename RHS>
auto modulo(LHS lhs, RHS rhs) {
  static_assert(std::is_integral<LHS>::value, "");
  static_assert(std::is_integral<RHS>::value, "");
  assert(rhs > 0);

  auto mod = lhs % static_cast<LHS>(rhs);
  if (mod < 0) {
    mod += rhs;
  }
  return mod;
}

} // namespace ungrd

#endif // UNGRD_MODULO_HPP_120D8B06A5034DE596B874E50925595E
