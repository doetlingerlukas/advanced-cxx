cmake_minimum_required(VERSION 3.16)

project(lit VERSION 1.0
            LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lit)
target_sources(lit 
  PRIVATE 
    src/lit.cpp
    src/command.cpp
)
target_include_directories(lit PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")