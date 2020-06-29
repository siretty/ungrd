#ifndef UNGRD_SET_HPP_17877B38DE4B4EBC8F009590A88D5072
#define UNGRD_SET_HPP_17877B38DE4B4EBC8F009590A88D5072

#include <boost/container/flat_set.hpp>
#include <boost/container/small_vector.hpp>

namespace ungrd {

template <typename K, typename C = std::less<K>, size_t N = 1>
using small_sort_set =
    boost::container::flat_set<K, C, boost::container::small_vector<K, N>>;

} // namespace ungrd

#endif // UNGRD_SET_HPP_17877B38DE4B4EBC8F009590A88D5072
