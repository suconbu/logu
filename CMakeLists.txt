cmake_minimum_required(VERSION 3.11)

project(logu VERSION 1.0)

if(MSVC)
    # To avoid warning D0925
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

include_directories(include)
if(UNIX)
    link_libraries(pthread)
endif()

add_executable(${PROJECT_NAME}
    example/example.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wshadow -Wundef)
endif()

enable_testing()
add_subdirectory(test)
