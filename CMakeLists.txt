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
target_link_libraries(VMds boost)
add_custom_target(run
  COMMAND VMds
)
