project(Echo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Echo STATIC)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
    set_target_properties(Echo PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${IncludeDir_glm}/*.hpp"
    "${IncludeDir_glm}/*.inl"
)

target_sources(Echo PRIVATE ${SOURCE_FILES})

target_precompile_headers(Echo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")

target_include_directories(Echo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_include_directories(Echo PRIVATE
    "${IncludeDir_spdlog}"
    "${IncludeDir_DirectX}"
    "${IncludeDir_glm}"
    "${IncludeDir_GLFW}"
    "${IncludeDir_VulkanSDK}"
    "${IncludeDir_entt}"
    "${IncludeDir_ImGui}"
)

target_compile_definitions(Echo PRIVATE
    "_CRT_SECURE_NO_WARNINGS"
    "GLM_FORCE_CTOR_INIT"
    $<$<CONFIG:Debug>:ECHO_DEBUG>
    $<$<CONFIG:Release>:ECHO_RELEASE>
    $<$<CONFIG:Dist>:ECHO_DIST>
)

if (WIN32)
    target_compile_definitions(Echo PRIVATE USING_DIRECTX_HEADERS)
endif()

target_link_libraries(Echo PRIVATE
    glfw
    imgui
    "${Library_Vulkan}"
    $<$<CONFIG:Debug>:${Library_SPIRV_Cross_Debug}>
    $<$<CONFIG:Debug>:${Library_SPIRV_Cross_GLSL_Debug}>
    $<$<CONFIG:Release>:${Library_SPIRV_Cross_Release}>
    $<$<CONFIG:Release>:${Library_SPIRV_Cross_GLSL_Release}>
    $<$<CONFIG:Dist>:${Library_SPIRV_Cross_Release}>
    $<$<CONFIG:Dist>:${Library_SPIRV_Cross_GLSL_Release}>
)
