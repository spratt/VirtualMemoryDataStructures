cmake_minimum_required (VERSION 3.4)
project (VMds)
set (VMds_VERSION_MAJOR 1)
set (VMds_VERSION_MINOR 0)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "-O2")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
find_package(Boost 1.59)
if(${Boost_FOUND})
    add_library(boost INTERFACE IMPORTED)
    set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
endif()
add_executable(VMds main.cc)
add_executable(radixtree_test radixtree_test.cc)
add_executable(radix_size_test radix_size_test.cc)
#add_executable(marray_test marray_test.cc)
target_link_libraries(VMds boost)
target_link_libraries(radix_size_test boost)
#target_link_libraries(marray_test boost)
set_target_properties(VMds PROPERTIES CXX_STANDARD 14)
set_target_properties(radixtree_test PROPERTIES CXX_STANDARD 14)
#set_target_properties(marray_test PROPERTIES
#  CXX_STANDARD 14
#  CMAKE_CXX_FLAGS_RELEASE "-O"
#  CMAKE_CXX_FLAGS ""
#  COMPILE_OPTIONS "-DNDEBUG -O"
#)
add_custom_target(run
  COMMAND VMds > ../output.csv
)
add_custom_target(runradix
  COMMAND radixtree_test
)
add_custom_target(runrs
  COMMAND radix_size_test
)
add_custom_target(valgrindradix
  COMMAND valgrind --leak-check=full ./radixtree_test
)
#add_custom_target(runmarray
#  COMMAND marray_test
#)

#add_custom_target(valgrindmarray
#  COMMAND valgrind --leak-check=full ./marray_test
#)
