cmake_minimum_required (VERSION 2.6)
project (test)


#include_directories("../include")

# add the executable
add_executable(test0 test.cpp)
add_executable(test1 x.cpp y.cpp)
add_executable(test2 new_tests.cpp)

set_property(TARGET test0 PROPERTY CXX_STANDARD 20)
set_property(TARGET test1 PROPERTY CXX_STANDARD 20)
set_property(TARGET test2 PROPERTY CXX_STANDARD 20)