cmake_minimum_required (VERSION 3.4)
project (VMds)
set (VMds_VERSION_MAJOR 1)
set (VMds_VERSION_MINOR 0)
find_package(Boost 1.59)
if(${Boost_FOUND})
    add_library(boost INTERFACE IMPORTED)
    set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
endif()
add_executable(VMds main.cc)
add_executable(radixtree_test radixtree_test.cc)
target_link_libraries(VMds boost)
set_target_properties(VMds PROPERTIES CXX_STANDARD 14)
set_target_properties(radixtree_test PROPERTIES CXX_STANDARD 14)
add_custom_target(run
  COMMAND VMds > ../output.csv
)
add_custom_target(runradix
  COMMAND radixtree_test
)
add_custom_target(valgrindradix
  COMMAND valgrind --leak-check=full ./radixtree_test
)
