cmake_minimum_required(VERSION 4.0)
project(softserve_academy)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CPM.cmake)

# include google test
CPMAddPackage(
    NAME GTest
    GITHUB_REPOSITORY google/googletest
    VERSION 1.17.0
)

function(add_task TASK_NAME)
    file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${TASK_NAME}/*.cpp)
    add_executable(${TASK_NAME} ${SOURCES})
    target_compile_definitions(${TASK_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        target_compile_options(${TASK_NAME} PRIVATE -march=native)
    endif()

    if(GTest_ADDED)
        target_link_libraries(${TASK_NAME} PRIVATE GTest::gtest_main)
        include(GoogleTest)
        gtest_discover_tests(${TASK_NAME})
    endif()
endfunction()

add_task(task1)
add_task(task2)
add_task(task3)