project(optimizer)
cmake_minimum_required(VERSION 2.6)


include_directories(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} .)

set(optimizer_SRC 
  optimizer.cpp
)

add_executable(optimizer ${optimizer_SRC} )
