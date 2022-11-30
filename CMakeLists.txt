cmake_minimum_required(VERSION 3.11)

project(clog VERSION 1.0)

add_executable(${PROJECT_NAME}
    example/example.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
if(UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
if(MSVC)
    string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wshadow -Wundef)
endif()

enable_testing()
add_subdirectory(test)
