# main
cmake_minimum_required(VERSION 3.14.0)

project(
  gan-ogl-engine
  VERSION 0.0.1
  LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENGINE_ENABLE_ASAN "Enable AddressSanitizer for Debug builds" OFF)
option(ENGINE_CGLTF_NO_STDIO "Build cgltf without stdio helpers" OFF)

include(FetchContent)

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
find_package(OpenGL REQUIRED)

# Jolt Physics
set(OVERRIDE_CXX_FLAGS OFF CACHE BOOL "" FORCE)
set(ENABLE_ALL_WARNINGS OFF CACHE BOOL "" FORCE)
set(INTERPROCEDURAL_OPTIMIZATION OFF CACHE BOOL "" FORCE)
set(ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  JoltPhysics
  GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
  GIT_TAG v5.5.0
  SOURCE_SUBDIR Build)
FetchContent_MakeAvailable(JoltPhysics)

# sources
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# build
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_compile_definitions(
  ${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:ENGINE_DEBUG>
                          $<$<CONFIG:RelWithDebInfo>:ENGINE_DEBUG>)

# Dear ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
add_library(
  imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(imgui PUBLIC glad glfw)

# cgltf
add_library(cgltf STATIC third_party/cgltf/cgltf_impl.cpp)
target_include_directories(cgltf
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cgltf)
if(ENGINE_CGLTF_NO_STDIO)
  target_compile_definitions(cgltf PUBLIC CGLTF_NO_STDIO)
endif()

# includes
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                          ${CMAKE_CURRENT_SOURCE_DIR}/include)

# link
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm::glm stb_image
                                              imgui cgltf Jolt OpenGL::GL)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wformat=2 -Wformat-security
                                                 -Werror=format-security)
endif()

if(ENGINE_ENABLE_ASAN AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>
            $<$<CONFIG:Debug>:-fno-omit-frame-pointer> $<$<CONFIG:Debug>:-g>)
  target_link_options(${PROJECT_NAME} PRIVATE
                      $<$<CONFIG:Debug>:-fsanitize=address>)
endif()

# os specific
if(WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa"
                                                "-framework IOKit"
                                                "-framework CoreVideo")
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_MACOS)
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE dl pthread)
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
endif()

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
  COMMENT "Deploying assets to build directory...")
