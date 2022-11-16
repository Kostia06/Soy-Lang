cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Soy_Lang)
SET(programName Slang)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeFiles/")
file(GLOB SOURCE src/*.cpp)
file(GLOB HEADER src/include/*.hpp)



add_executable(Soy_Lang ${HEADER} ${SOURCE} )





