cmake_minimum_required(VERSION 3.31.3)
project(Echo)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH x64)
else()
    set(ARCH x86)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "Configs" FORCE)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

foreach(CONFIG_TYPE Debug Release Dist)
    string(TOUPPER ${CONFIG_TYPE} CONFIG_UPPER)
    if(NOT DEFINED CMAKE_EXE_LINKER_FLAGS_${CONFIG_UPPER})
        set(CMAKE_EXE_LINKER_FLAGS_${CONFIG_UPPER} "")
    endif()
endforeach()

set(ENABLE_SPIRV_CODEGEN ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Minecraft_Clone)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(Echo/vendor/GLFW)
set_target_properties(glfw PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo/vendor/imgui)
set_target_properties(ImGui PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo/vendor/vk-bootstrap)
set_target_properties(vk-bootstrap PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo/vendor/DirectXTK12)
set_target_properties(DirectXTK12 PROPERTIES FOLDER "Dependencies")

set(SLANG_ENABLE_SLANG_RHI FALSE)
set(SLANG_USE_SYSTEM_VULKAN_HEADERS TRUE)

set(SLANG_ENABLE_SLANG_GLSLANG FALSE)
set(SLANG_ENABLE_GFX FALSE)
set(SLANG_ENABLE_SLANGD FALSE)
set(SLANG_ENABLE_SLANGC FALSE)
set(SLANG_ENABLE_SLANGRT FALSE)
set(SLANG_ENABLE_TESTS FALSE)
set(SLANG_ENABLE_EXAMPLES FALSE)

add_subdirectory(Echo/vendor/Slang)
set_target_properties(slang PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo/vendor/yaml)
set_target_properties(yaml-cpp PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo/vendor/ImGuizmo)
set_target_properties(ImGuizmo PROPERTIES FOLDER "Dependencies")

set_target_properties(yaml-cpp-parse PROPERTIES FOLDER "Dependencies")
set_target_properties(yaml-cpp-read PROPERTIES FOLDER "Dependencies")
set_target_properties(yaml-cpp-sandbox PROPERTIES FOLDER "Dependencies")
set_target_properties(slang-common-objects PROPERTIES FOLDER "Dependencies")
set_target_properties(core PROPERTIES FOLDER "Dependencies")
set_target_properties(compiler-core PROPERTIES FOLDER "Dependencies")

add_subdirectory(Echo)
add_subdirectory(Horizon)

