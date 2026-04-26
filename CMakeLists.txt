# main
cmake_minimum_required(VERSION 3.14.0)

project(gan-ogl-engine VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image
 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/include
)

# glad
add_library(glad STATIC third_party/glad/src/gl.c)
target_include_directories(glad PUBLIC third_party/glad/include)

# other
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

# sources
file(GLOB_RECURSE SOURCE_FILES
 "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# build
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# includes
target_include_directories(${PROJECT_NAME}
 PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# link
target_link_libraries(${PROJECT_NAME}
 PRIVATE
  glad
  glfw
  glm::glm
  stb_image
)

# os specific
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME}
  PRIVATE
   "-framework Cocoa"
   "-framework OpenGL"
   "-framework IOKit"
   "-framework CoreVideo"
 )
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_MACOS)
    add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets
   $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
 )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE GL dl pthread)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
    add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets
   $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
 )
endif()
