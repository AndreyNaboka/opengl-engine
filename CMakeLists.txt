cmake_minimum_required(VERSION 3.14.0)
project(opengl-engine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/ext 
)

set(HEADER_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ext  
)

set(SOURCES)
foreach(DIR IN LISTS SOURCE_DIRS)
    file(GLOB_RECURSE DIR_SOURCES
        "${DIR}/*.cpp"
        "${DIR}/*.cxx"
        "${DIR}/*.cc"
        "${DIR}/*.c"
    )
    list(APPEND SOURCES ${DIR_SOURCES})
endforeach()

set(HEADERS)
foreach(DIR IN LISTS HEADER_DIRS)
    file(GLOB_RECURSE DIR_HEADERS
        "${DIR}/*.h"
        "${DIR}/*.hpp"
        "${DIR}/*.hxx"
    )
    list(APPEND HEADERS ${DIR_HEADERS})
endforeach()

add_executable(opengl-engine ${SOURCES} ${HEADERS})

target_include_directories(opengl-engine PUBLIC ${HEADER_DIRS})

target_link_libraries(opengl-engine
    PUBLIC
        "-framework Metal"
        "-framework Foundation"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework OpenGL"
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/lib-arm64/libglfw3.a
)

target_compile_options(opengl-engine PRIVATE -Wno-deprecated-declarations)