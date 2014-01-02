cmake_minimum_required(VERSION 2.8)

project(CamelBabelServer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/;${CMAKE_MODULE_PATH}")

message(STATUS ${CMAKE_MODULE_PATH})

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

### PTHREAD
target_link_libraries(server pthread)

### BOOST
add_definitions(-DBOOST_THREAD_USE_LIB=1)
find_package(Boost 1.49.0 COMPONENTS system thread REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(server ${Boost_LIBRARIES})

### MONGO
find_package(MONGO REQUIRED)
include_directories(${MONGO_INCLUDE_DIR})
target_link_libraries(server ${MONGO_LIBRARIES})