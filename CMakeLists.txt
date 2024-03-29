cmake_minimum_required(VERSION 3.9)
project(42sh)

set(CMAKE_C_STANDARD 99)

add_compile_options(-Wall -Wextra -pedantic -Werror -g -D_GNU_SOURCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

### 42SH EXECUTABLE

include_directories(src)
include_directories(tests)

file(GLOB_RECURSE SRC_FILES  src/*)
add_executable(42sh src/42sh.c)

add_library(42sh_lib SHARED ${SRC_FILES})
target_link_libraries(42sh 42sh_lib -fsanitize=address readline)

### UNIT TESTS

file(GLOB_RECURSE TEST_FILES tests/*)

add_library(test_42sh SHARED ${SRC_FILES} ${TEST_FILES})
target_link_libraries(test_42sh 42sh_lib)

add_custom_target(doc doxygen ../doxy-config)
add_custom_target(debug python3 ../tests/test_suite.py --debug)
add_custom_target(check python3 ../tests/test_suite.py)
add_custom_target(style python3 ../tests/test_suite.py --style)
add_custom_target(unit python3 ../tests/unit_test.py)
