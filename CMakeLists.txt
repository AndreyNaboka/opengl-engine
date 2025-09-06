cmake_minimum_required(VERSION 3.14.0)
project(opengl-engine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(opengl-engine
  src/main.cpp
  src/glad.c
)

target_include_directories(
  opengl-engine
  PRIVATE
  libs/glfw/include  # путь к заголовочным файлам GLFW
  include
)

target_link_libraries(opengl-engine 
  PRIVATE 
  "-framework Metal -framework Foundation -framework Cocoa -framework IOKit -framework OpenGL"
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/lib-arm64/libglfw3.a
)

target_compile_options(opengl-engine PRIVATE -Wno-deprecated-declarations)
