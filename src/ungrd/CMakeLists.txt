
find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS container)


# copy the parallel hashmap into our source tree
file(
    CREATE_LINK
    ${PROJECT_SOURCE_DIR}/external/parallel-hashmap/parallel_hashmap
    ${CMAKE_CURRENT_SOURCE_DIR}/cxx/phmap
    COPY_ON_ERROR SYMBOLIC
)


add_library(
    ungrd INTERFACE
)

target_sources(
    ungrd INTERFACE

    cxx/assert.hpp
    cxx/modulo.hpp
    cxx/lexicographic_indexing.hpp

    cxx/map.hpp
    cxx/set.hpp

    compact_grid.hpp
    compact_multi_grid.hpp

    cell_policy.hpp

    entry_policy.hpp
    space_policy.hpp

    dense_grid.hpp
    compact_grid.hpp
)
target_include_directories(
    ungrd

    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(
    ungrd

    INTERFACE OpenMP::OpenMP_CXX Boost::container
)


if (UNGRD_BUILD_TESTS)
  add_executable(
      ungrd-tests

      cxx/modulo.tests.cpp
      cxx/static_bitset.tests.cpp

      grid.tests.hpp
      dense_grid.tests.cpp
      compact_grid.tests.cpp
      compact_grid.tests.cpp)
  target_link_libraries(
      ungrd-tests

      ungrd
      ${UNGRD_GOOGLETEST_LINK_LIBRARIES}
  )
endif ()

if (UNGRD_BUILD_BENCHMARKS)
  add_executable(
      ungrd-benchmarks

      cxx/static_bitset.bench.cpp
      object_pool.bench.cpp

      grid.bench.hpp
      dense_grid.bench.cpp
      compact_grid.bench.cpp
  )
  target_link_libraries(
      ungrd-benchmarks

      ungrd
      ${UNGRD_BENCHMARK_LINK_LIBRARIES}
  )
endif ()
