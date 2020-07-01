
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
    cxx/map.hpp
    cxx/set.hpp

    compact_grid.hpp
    compact_multi_grid.hpp
    uniform_grid.cpp uniform_grid.hpp
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
