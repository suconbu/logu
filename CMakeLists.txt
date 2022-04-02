cmake_minimum_required(VERSION 3.8)
project(example VERSION 1.0)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}
    example/example.cpp
    example/example_severityfilter.cpp
    # example/example_tagfilter.cpp
    example/example_disable.cpp
    example/example_fileoutput.cpp
    example/example_outputhandler.cpp
    example/example_instance.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE include)

add_executable(${PROJECT_NAME}_test test/test.cpp)
target_include_directories(${PROJECT_NAME}_test PRIVATE include)

target_link_libraries(${PROJECT_NAME}_test gtest_main)
add_test(NAME test COMMAND ${PROJECT_NAME}_test)
enable_testing()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wshadow -Wundef)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exe)
