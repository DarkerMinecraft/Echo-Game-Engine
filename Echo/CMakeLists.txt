cmake_minimum_required(VERSION 3.31.3)
project(Echo)
set(CMAKE_CXX_STANDARD 20)

target_precompile_headers(Echo PRIVATE src/pch.h)

file(GLOB_RECURSE SRC_FILES
    "src/**.h"
    "src/**.cpp"
    "vendor/glm/**.hpp"
    "vendor/glm/**.inl"
)

target_include_directories(Echo PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/Echo/vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/DirectX/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/glm
    ${CMAKE_SOURCE_DIR}/Echo/vendor/GLFW/include
    $ENV{VULKAN_SDK}
    ${CMAKE_SOURCE_DIR}/Echo/vendor/entt/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/imgui
    ${CMAKE_SOURCE_DIR}/Echo/vendor/vk-bootstrap/src
    ${CMAKE_SOURCE_DIR}/Echo/vendor/VulkanMemoryAllocator/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/tinyobjloader
    ${CMAKE_SOURCE_DIR}/Echo/vendor/stb
)

link_directories(${VULKAN_SDK}/Lib)

target_link_libraries(Echo PRIVATE
    GLFW
    ImGui
    $ENV{VULKAN_SDK}/vulkan-1.lib
    vk-bootstrap
    Slang
)

target_sources(Echo PRIVATE ${SRC_FILES} src/pch.cpp)

find_package(nuget CONFIG REQUIRED)
nuget_restore(Echo directxtk12_desktop_2019 2024.6.5.1)

if(WIN32)
    target_compile_definitions(Echo PRIVATE USING_DIRECTX_HEADERS)
endif()


foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Echo PRIVATE $<$<CONFIG:${CONFIG}>:ECHO_${CONFIG}>)
endforeach()

set_target_properties(Echo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Echo
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/Echo
)