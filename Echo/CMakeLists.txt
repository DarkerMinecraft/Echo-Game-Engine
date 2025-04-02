cmake_minimum_required(VERSION 3.31.3)
project(Echo)
set(CMAKE_CXX_STANDARD 20)

add_library(Echo STATIC)

target_precompile_headers(Echo PRIVATE src/pch.h)

file(GLOB_RECURSE SRC_FILES
    "src/**.h"
    "src/**.cpp"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${SRC_FILES})

target_include_directories(Echo PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/Echo/vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/DirectX/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/glm
    $ENV{VULKAN_SDK}
    ${CMAKE_SOURCE_DIR}/Echo/vendor/entt/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/imgui
    ${CMAKE_SOURCE_DIR}/Echo/vendor/vk-bootstrap/src
    ${CMAKE_SOURCE_DIR}/Echo/vendor/VulkanMemoryAllocator/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/tinyobjloader
    ${CMAKE_SOURCE_DIR}/Echo/vendor/stb
    ${CMAKE_SOURCE_DIR}/Echo/vendor/DirectXTK12/Inc
    ${CMAKE_SOURCE_DIR}/Echo/vendor/Slang/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/uuidv4
    ${CMAKE_SOURCE_DIR}/Echo/vendor/yaml/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/ImGuizmo
)

link_directories(${VULKAN_SDK}/Lib)

target_link_libraries(Echo PRIVATE
    ImGui
    $ENV{VULKAN_SDK}/Lib/vulkan-1.lib
    $ENV{VULKAN_SDK}/Lib/dxcompiler.lib
    vk-bootstrap
    DirectXTK12
    slang
    yaml-cpp
    ImGuizmo
)

target_sources(Echo PRIVATE 
    ${SRC_FILES} 
    src/pch.cpp
)

if(WIN32)
    target_compile_definitions(Echo PRIVATE USING_DIRECTX_HEADERS)
    target_compile_definitions(Echo PRIVATE EC_PLATFORM_WINDOWS)
endif()

foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Echo PRIVATE $<$<CONFIG:${CONFIG}>:ECHO_${CONFIG}>)
endforeach()

set_target_properties(Echo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Echo
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/Echo
)