cmake_minimum_required(VERSION 3.31.3)
project(EchoEngine)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH x64)
else()
    set(ARCH x86)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "Configs" FORCE)

# Add UTF-8 support
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

foreach(CONFIG_TYPE Debug Release Dist)
    string(TOUPPER ${CONFIG_TYPE} CONFIG_UPPER)
    if(NOT DEFINED CMAKE_EXE_LINKER_FLAGS_${CONFIG_UPPER})
        set(CMAKE_EXE_LINKER_FLAGS_${CONFIG_UPPER} "")
    endif()
endforeach()

# Global output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# IDE settings
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Horizon)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_VS_JUST_MY_CODE_DEBUGGING ON)
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 0)

# Create VendorLibs custom target that will build all vendors
add_custom_target(VendorLibs)

# Dependencies - build them and add to VendorLibs
add_subdirectory(Echo/vendor/imgui ${CMAKE_BINARY_DIR}/vendor_build/imgui)
set_target_properties(ImGui PROPERTIES FOLDER "Dependencies")
add_dependencies(VendorLibs ImGui)

add_subdirectory(Echo/vendor/vk-bootstrap ${CMAKE_BINARY_DIR}/vendor_build/vk-bootstrap)
set_target_properties(vk-bootstrap PROPERTIES FOLDER "Dependencies")
add_dependencies(VendorLibs vk-bootstrap)

add_subdirectory(Echo/vendor/DirectXTK12 ${CMAKE_BINARY_DIR}/vendor_build/DirectXTK12)
set_target_properties(DirectXTK12 PROPERTIES FOLDER "Dependencies")
add_dependencies(VendorLibs DirectXTK12)

# Slang configuration
set(SLANG_ENABLE_SLANG_RHI FALSE)
set(SLANG_USE_SYSTEM_VULKAN_HEADERS TRUE)
set(SLANG_ENABLE_SLANG_GLSLANG FALSE)
set(SLANG_ENABLE_GLSLANG FALSE)  # Fix for the Slang issue
set(SLANG_ENABLE_GFX FALSE)
set(SLANG_ENABLE_SLANGD FALSE)
set(SLANG_ENABLE_SLANGC FALSE)
set(SLANG_ENABLE_SLANGRT FALSE)
set(SLANG_ENABLE_TESTS FALSE)
set(SLANG_ENABLE_EXAMPLES FALSE)

add_subdirectory(Echo/vendor/Slang ${CMAKE_BINARY_DIR}/vendor_build/Slang)
set_target_properties(slang PROPERTIES FOLDER "Dependencies")
add_dependencies(VendorLibs slang)

add_subdirectory(Echo/vendor/yaml ${CMAKE_BINARY_DIR}/vendor_build/yaml)
set_target_properties(yaml-cpp PROPERTIES FOLDER "Dependencies")
add_dependencies(VendorLibs yaml-cpp)

add_subdirectory(Echo/vendor/ImGuizmo ${CMAKE_BINARY_DIR}/vendor_build/ImGuizmo)
set_target_properties(ImGuizmo PROPERTIES FOLDER "Dependencies")
add_dependencies(VendorLibs ImGuizmo)

add_subdirectory(Echo/vendor/Box2D ${CMAKE_BINARY_DIR}/vendor_build/Box2D)
set_target_properties(box2d PROPERTIES FOLDER "Dependencies")
add_dependencies(VendorLibs box2d)

# Handle any dependency sub-targets
set_target_properties(yaml-cpp-parse PROPERTIES FOLDER "Dependencies")
set_target_properties(yaml-cpp-read PROPERTIES FOLDER "Dependencies")
set_target_properties(yaml-cpp-sandbox PROPERTIES FOLDER "Dependencies")
set_target_properties(slang-common-objects PROPERTIES FOLDER "Dependencies")
set_target_properties(core PROPERTIES FOLDER "Dependencies")
set_target_properties(compiler-core PROPERTIES FOLDER "Dependencies")
set_target_properties(slangi PROPERTIES FOLDER "Dependencies")

# Create Clean targets for Echo and Horizon
add_custom_target(CleanEcho
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/Echo"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/bin/Debug/Echo"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/bin/Release/Echo"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/bin/Dist/Echo"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/lib/Debug/Echo"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/lib/Release/Echo"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/lib/Dist/Echo"
    COMMENT "Cleaning Echo project files"
)

add_custom_target(CleanHorizon
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/Horizon"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/bin/Debug/Horizon"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/bin/Release/Horizon"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/bin/Dist/Horizon"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/lib/Debug/Horizon"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/lib/Release/Horizon"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/lib/Dist/Horizon"
    COMMENT "Cleaning Horizon project files"
)

# Create a target to clean both Echo and Horizon
add_custom_target(CleanEchoProjects
    DEPENDS CleanEcho CleanHorizon
    COMMENT "Cleaning Echo and Horizon project files"
)

# Main projects - make them depend on VendorLibs
add_subdirectory(Echo)
add_dependencies(Echo VendorLibs)

add_subdirectory(Horizon)
add_dependencies(Horizon Echo)