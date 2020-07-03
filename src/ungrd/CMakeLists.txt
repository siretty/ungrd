
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
    ungrd

    cxx/assert.hpp
    cxx/modulo.hpp
    cxx/ndarray.hpp

    cxx/map.hpp
    cxx/set.hpp

    compact_grid.hpp
    compact_multi_grid.hpp

    uniform_grid.cpp uniform_grid.hpp

    cell_policy.hpp

    entry_policy.hpp
    space_policy.hpp

    dense_grid.hpp
)
target_include_directories(
    ungrd

    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(
    ungrd

    PUBLIC OpenMP::OpenMP_CXX Boost::container
)
set_target_properties(ungrd PROPERTIES LINKER_LANGUAGE CXX)

add_executable(
    ungrd-tests

    cxx/modulo.tests.cpp
    dense_grid.tests.cpp
)
target_link_libraries(
    ungrd-tests

    ungrd
    ${UNGRD_GOOGLETEST_LINK_LIBRARIES}
)
