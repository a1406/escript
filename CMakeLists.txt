CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(main)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "./cmake/")

FILE(GLOB SRC_LIST
  "./*.cpp"
  )

add_definitions(-g -O0 -Wno-write-strings)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable  (main ${SRC_LIST})

