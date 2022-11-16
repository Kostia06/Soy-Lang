cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Soy_Lang)
SET(programName Slang)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeFiles/")
file(GLOB SOURCE src/*.cpp)
file(GLOB HEADER src/include/*.hpp)



add_executable(Soy_Lang ${HEADER} ${SOURCE} )

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)


include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost REQUIRED COMPONENTS system) 

include_directories(${Boost_INCLUDE_DIRS}) 

target_link_libraries(Soy_Lang m ${Boost_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})




