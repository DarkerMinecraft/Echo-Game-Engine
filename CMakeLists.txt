cmake_minimum_required(VERSION 3.31.3)
project(Echo)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH x64)
else()
    set(ARCH x86)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "Configs" FORCE)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Minecraft_Clone)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(Echo/vendor/GLFW)
set_target_properties(GLFW PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo/vendor/imgui)
set_target_properties(imgui PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo/vendor/vk-bootstrap)
set_target_properties(vk-bootstrap PROPERTIES FOLDER "Dependencies")

set(SLANG_USE_SYSTEM_MINIZ ON CACHE BOOL "" FORCE)
set(SLANG_USE_SYSTEM_VULKAN_HEADERS ON CACHE BOOL "" FORCE)
set(SLANG_USE_SYSTEM_UNORDERED_DENSE ON CACHE BOOL "" FORCE)

set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_GFX OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SLANG_RHI OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_LLVM_FLAVOR DISABLE CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SLANG_GLSLANG OFF CACHE BOOL "" FORCE)

set(SLANG_ENABLE_SLANGD ON CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SLANGRT ON CACHE BOOL "" FORCE)
add_subdirectory(Echo/vendor/Slang)
set_target_properties(Slang PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo)
add_subdirectory(Echo_Editor)
add_subdirectory(Minecraft_Clone)
