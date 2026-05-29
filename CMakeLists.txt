# main
cmake_minimum_required(VERSION 3.14.0)

project(
  gan-ogl-engine
  VERSION 0.0.1
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# stb_image
add_library(stb_image INTERFACE)
target_include_directories(
  stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/include)

# glad
add_library(glad STATIC third_party/glad/src/gl.c)
target_include_directories(glad PUBLIC third_party/glad/include)

# other
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

# sources
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# build
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:ENGINE_DEBUG>
)

# Dear ImGui 
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(imgui PUBLIC glad glfw)

# cgltf
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/cgltf
)
target_sources(${PROJECT_NAME} PRIVATE
    third_party/cgltf/cgltf_impl.cpp
)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE CGLTF_NO_STDIO)
endif()

# includes
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                          ${CMAKE_CURRENT_SOURCE_DIR}/include)

# link
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm::glm stb_image imgui)

target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wformat=2 
    -Wformat-security
    -Werror=format-security 
)

# sanitizer
#target_compile_options(${PROJECT_NAME} PRIVATE
#    $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -g>
#)
#target_link_options(${PROJECT_NAME} PRIVATE
#    $<$<CONFIG:Debug>:-fsanitize=address>
#)

# os specific
if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
  target_link_libraries(
    ${PROJECT_NAME} PRIVATE "-framework Cocoa" "-framework OpenGL"
                            "-framework IOKit" "-framework CoreVideo")
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_MACOS)
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE GL dl pthread)
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
endif()

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
  COMMENT "Deploying assets to build directory...")
