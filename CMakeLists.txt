cmake_minimum_required(VERSION 3.8)
project(example VERSION 1.0)

add_executable(${PROJECT_NAME}
    example/example.cpp
    example/example_severityfilter.cpp
    example/example_tagfilter.cpp
    example/example_disable.cpp
    example/example_fileoutput.cpp
    example/example_outputhandler.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wshadow -Wundef)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exe)
