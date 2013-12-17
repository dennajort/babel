cmake_minimum_required(VERSION 2.8)

project(CamelBabelServer)

file(
  GLOB_RECURSE
  SERVER_SOURCES
  src/*.cpp
  )

file(
  GLOB_RECURSE
  SERVER_HEADERS
  include/*.hpp
  include/*.hh
  )

add_executable(server ${SERVER_SOURCES} ${SERVER_HEADERS})

FIND_PACKAGE(Boost 1.40.0 COMPONENTS system REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES})

