project(EchoEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories for EchoEditor project
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/EchoEditor)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/EchoEditor)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/EchoEditor)

# Define the EchoEditor executable
add_executable(EchoEditor)

# Add all source files from the src folder under EchoEditor, maintaining structure
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/**.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp")
target_sources(EchoEditor PRIVATE ${SOURCE_FILES})

# Include directories for EchoEditor
target_include_directories(EchoEditor PRIVATE 
                            "${CMAKE_CURRENT_SOURCE_DIR}/src" 
                            "${CMAKE_SOURCE_DIR}/Echo/src" 
                            "${IncludeDir_glm}" 
                            "${IncludeDir_imgui}" 
                            "${IncludeDir_entt}"
                            "${IncludeDir_spdlog}"
)

# Link the Echo library to EchoEditor
target_link_libraries(EchoEditor PRIVATE Echo)

target_compile_definitions(EchoEditor PRIVATE
    $<$<CONFIG:Debug>:EC_DEBUG>
    $<$<CONFIG:Release>:EC_RELEASE>
    $<$<CONFIG:Dist>:EC_DIST>
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Dist")
    target_compile_options(EchoEditor PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/O2>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3>
    )
endif()