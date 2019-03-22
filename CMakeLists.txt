

cmake_minimum_required(VERSION 3.13)
project(mpi_lab4_2)
include_directories(/usr/include/mpich/)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
add_executable(mpi_lab4_2 main.cpp)
find_package(MPI REQUIRED)
target_link_libraries(mpi_lab4_2 ${MPI_LIBRARIES})
