cmake_minimum_required(VERSION 2.8)

project(CamelBabelServer)

file(
  GLOB_RECURSE
  SOURCES
  src/*.cpp
  )

file(
  GLOB_RECURSE
  HEADERS
  include/*.hh
  )

include_directories(include)

add_executable(server ${SOURCES} ${HEADERS})

find_package(Boost 1.40.0 COMPONENTS system REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(server ${Boost_LIBRARIES})

