
find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS container)

add_library(
    ungrd

    cxx/assert.hpp
    cxx/map.hpp
    cxx/set.hpp

    compact_grid.hpp
    compact_multi_grid.hpp
)
target_include_directories(
    ungrd

    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(
    ungrd

    PUBLIC OpenMP::OpenMP_CXX Boost::container absl::flat_hash_map
)
