cmake_minimum_required(VERSION 3.14.0)
project(opengl-engine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем директории с исходниками и заголовками
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Собираем все исходные файлы
file(GLOB_RECURSE SOURCES 
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.cxx"
    "${SOURCE_DIR}/*.cc"
    "${SOURCE_DIR}/*.c"
)

file(GLOB_RECURSE HEADERS 
    "${HEADER_DIR}/*.h"
    "${HEADER_DIR}/*.hpp"
    "${HEADER_DIR}/*.hxx"
)

# Добавляем файлы в проект
add_executable(opengl-engine ${SOURCES} ${HEADERS})

target_include_directories(opengl-engine PUBLIC ${HEADER_DIR})

target_link_libraries(opengl-engine 
  PUBLIC 
  "-framework Metal -framework Foundation -framework Cocoa -framework IOKit -framework OpenGL"
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/lib-arm64/libglfw3.a
)

target_compile_options(opengl-engine PRIVATE -Wno-deprecated-declarations)
