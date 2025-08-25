cmake_minimum_required(VERSION 3.14.0)

project(opengl-engine, VERSION 0.0.1)

set(CMAKE_CXX_STANDART, 20)

link_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable(opengl-engine
  src/main.cpp
)

target_include_directories(
  opengl-engine
  PRIVATE
  libs/glfw/include
)

target_link_libraries(opengl-engine 
  PRIVATE 
  "-framework Metal -framework Foundation -framework Cocoa -framework IOKit -framework OpenGL"
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/lib-arm64/libglfw3.a)

