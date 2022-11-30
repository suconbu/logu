cmake_minimum_required(VERSION 3.11)
project(clog_test)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    GIT_TAG "release-1.12.1"
)
option(BUILD_GMOCK "" OFF)
option(INSTALL_GTEST "" OFF)
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/test.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main pthread)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
endif()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
