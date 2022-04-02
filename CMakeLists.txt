cmake_minimum_required(VERSION 3.8)

project(clog VERSION 1.0)

add_executable(${PROJECT_NAME}
    example/example.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
if(UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

set(TEST_NAME ${PROJECT_NAME}_test)
add_executable(${TEST_NAME} test/test.cpp)

option(BUILD_GMOCK "Builds the googlemock subprojects" OFF)
option(INSTALL_GTEST "Enables installation of googletest" OFF)
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

target_include_directories(${TEST_NAME} PRIVATE include)
target_link_libraries(${TEST_NAME} gtest_main)
if(UNIX)
    target_link_libraries(${TEST_NAME} pthread)
endif()

add_test(NAME test COMMAND ${TEST_NAME})
enable_testing()

add_subdirectory(googletest)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wshadow -Wundef)
endif()
