cmake_minimum_required(VERSION 3.16)
project(ungrd C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
)

option(UNGRD_BUILD_TESTS "Build the tests" ON)
option(UNGRD_FETCH_GOOGLETEST "Fetch googletest automatically" ON)
if (UNGRD_BUILD_TESTS)
  enable_testing()

  if (UNGRD_FETCH_GOOGLETEST)
    FetchContent_MakeAvailable(googletest)
    set_target_properties(gtest PROPERTIES FOLDER external)
    set_target_properties(gtest_main PROPERTIES FOLDER external)
    set_target_properties(gmock PROPERTIES FOLDER external)
    set_target_properties(gmock_main PROPERTIES FOLDER external)
    set(UNGRD_GOOGLETEST_LINK_LIBRARIES gtest gtest_main)
  else ()
    find_package(GTest)
    set(UNGRD_GOOGLETEST_LINK_LIBRARIES GTest::gtest GTest::gtest_main)
  endif ()
endif ()

add_subdirectory(src)
