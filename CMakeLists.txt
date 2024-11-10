cmake_minimum_required(VERSION 3.21)

project(EchoSolution)

set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)

# Set common output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include dependencies
include("${CMAKE_SOURCE_DIR}/Dependencies.cmake")

# Add vendor libraries
add_subdirectory("Echo/vendor/GLFW")
set_property(TARGET glfw PROPERTY FOLDER "Dependencies")

add_subdirectory("Echo/vendor/imgui")
set_property(TARGET ImGui PROPERTY FOLDER "Dependencies")

add_subdirectory("Echo/vendor/DirectX")
set_property(TARGET DirectX-Headers PROPERTY FOLDER "Dependencies")

add_subdirectory("Echo/vendor/DirectXTK12")
set_property(TARGET DirectXTK12 PROPERTY FOLDER "Dependencies")

add_subdirectory("Echo")
add_subdirectory("EchoEditor")
