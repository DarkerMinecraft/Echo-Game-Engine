cmake_minimum_required(VERSION 3.31.3)
project(Echo)
set(CMAKE_CXX_STANDARD 20)

set(ECHO_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(Echo STATIC)
target_precompile_headers(Echo PRIVATE ${ECHO_DIR}/src/Echo/pch.h)

# Find all source files
file(GLOB_RECURSE SRC_FILES
    "${ECHO_DIR}/src/**.h"
    "${ECHO_DIR}/src/**.cpp"
)

source_group(TREE "${ECHO_DIR}/src" PREFIX "Source" FILES ${SRC_FILES})

target_include_directories(Echo 
    PUBLIC
    ${ECHO_DIR}/src/Echo
    ${ECHO_DIR}/vendor/spdlog/include
    ${ECHO_DIR}/vendor/glm
    ${ECHO_DIR}/vendor/entt/include
    ${ECHO_DIR}/vendor/imgui
    ${ECHO_DIR}/vendor/ImGuizmo
    PRIVATE
    ${ECHO_DIR}/src/Platform
    ${ECHO_DIR}/vendor/DirectX/include
    $ENV{VULKAN_SDK}
    ${ECHO_DIR}/vendor/vk-bootstrap/src
    ${ECHO_DIR}/vendor/VulkanMemoryAllocator/include
    ${ECHO_DIR}/vendor/tinyobjloader/include
    ${ECHO_DIR}/vendor/stb
    ${ECHO_DIR}/vendor/DirectXTK12/Inc
    ${ECHO_DIR}/vendor/Slang/include
    ${ECHO_DIR}/vendor/yaml/include
    ${ECHO_DIR}/vendor/Box2D/include
)

# Link with dependencies
target_link_libraries(Echo PUBLIC
    ImGui
    $ENV{VULKAN_SDK}/Lib/vulkan-1.lib
    $ENV{VULKAN_SDK}/Lib/dxcompiler.lib
    vk-bootstrap
    DirectXTK12
    slang
    yaml-cpp
    ImGuizmo
    box2d
)

target_sources(Echo PRIVATE 
    ${SRC_FILES} 
)

# Output directories
set_target_properties(Echo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Echo
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/Echo
)