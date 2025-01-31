cmake_minimum_required(VERSION 3.31.3)
project(Echo)
set(CMAKE_CXX_STANDARD 20)

add_library(Echo STATIC)

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
    ${CMAKE_SOURCE_DIR}/Echo/vendor/DirectXTK12/Inc
)

link_directories(${VULKAN_SDK}/Lib)

target_link_libraries(Echo PRIVATE
    glfw
    ImGui
    $ENV{VULKAN_SDK}/Lib/vulkan-1.lib
    $ENV{VULKAN_SDK}/Lib/dxcompiler.lib
    vk-bootstrap
    DirectXTK12

)
target_sources(Echo PRIVATE ${SRC_FILES} src/pch.cpp "src/Platform/Shader/ShaderCompiler.h" "src/Platform/Shader/ShaderCompiler.cpp" "src/Platform/Vulkan/VulkanBuffer.cpp" "src/Echo/ImGui/ImGuiBuild.cpp" "src/Platform/Vulkan/VulkanMesh.h" "src/Echo/Graphics/Mesh.h" "src/Platform/Vulkan/VulkanMesh.cpp")

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