#ifndef UNGRD_ASSERT_HPP_FD78E506F53343C6B0FB17811F1B296A
#define UNGRD_ASSERT_HPP_FD78E506F53343C6B0FB17811F1B296A

#include <cassert>

#define UNGRD_ASSERT(MESSAGE, ...)                                             \
  (static_cast<bool>(__VA_ARGS__) ? (void) (0) : assert(not #MESSAGE))

#define UNGRD_ASSERT_IN_RANGE(FIRST, VALUE, LAST)                              \
  UNGRD_ASSERT(FIRST <= VALUE < LAST,                                          \
               [](auto const f, auto const value, auto const l) {              \
                 using value_type = decltype(value);                           \
                 auto const first = static_cast<value_type>(f);                \
                 auto const last = static_cast<value_type>(l);                 \
                 return (first <= value) and (value < last);                   \
               }((FIRST), (VALUE), (LAST)))

#endif//UNGRD_ASSERT_HPP_FD78E506F53343C6B0FB17811F1B296A
