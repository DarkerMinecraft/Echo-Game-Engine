project(Echo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories for Echo project
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Echo)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Echo)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Echo)

# Define the Echo library
add_library(Echo STATIC)

# Add all source files from the src folder under Echo, maintaining structure
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/**.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp")
target_sources(Echo PRIVATE ${SOURCE_FILES})

# Include directories for Echo
target_include_directories(Echo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(Echo PRIVATE 
                            "${IncludeDir_glm}" 
                            "${IncludeDir_GLFW}" 
                            "${IncludeDir_VulkanSDK}" 
                            "${IncludeDir_imgui}" 
                            "${IncludeDir_entt}"
                            "${IncludeDir_spdlog}"
                            "${IncludeDir_DirectX}"
                            "${IncludeDir_DirectXTK}"
)

if(WIN32)
    add_definitions(-DECHO_PLATFORM_WINDOWS)
    add_definitions(-DUSING_DIRECTX_HEADERS)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DECHO_PLATFORM_LINUX)
elseif(APPLE)
    add_definitions(-DECHO_PLATFORM_MACOS)
endif()

target_compile_definitions(Echo PRIVATE
    "_CRT_SECURE_NO_WARNINGS"
    "GLM_FORCE_CTOR_INIT"
    $<$<CONFIG:Debug>:ECHO_DEBUG>
    $<$<CONFIG:Release>:ECHO_RELEASE>
    $<$<CONFIG:Dist>:ECHO_DIST>
)

# Link required libraries
target_link_libraries(Echo PRIVATE 
                        glfw 
                        imgui 
                        "${Library_Vulkan}"
                        $<$<CONFIG:Debug>:${Library_SPIRV_Cross_Debug}>
                        $<$<CONFIG:Debug>:${Library_SPIRV_Cross_GLSL_Debug}>
                        $<$<CONFIG:Release>:${Library_SPIRV_Cross_Release}>
                        $<$<CONFIG:Release>:${Library_SPIRV_Cross_GLSL_Release}>
                        $<$<CONFIG:Dist>:${Library_SPIRV_Cross_Release}>
                        $<$<CONFIG:Dist>:${Library_SPIRV_Cross_GLSL_Release}>)
