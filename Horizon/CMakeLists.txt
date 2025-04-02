cmake_minimum_required(VERSION 3.31.3)
project(Horizon)
set(CMAKE_CXX_STANDARD 20)
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(TARGET_DIR $<TARGET_FILE_DIR:Horizon>)

# Create the executable
add_executable(Horizon)
set_target_properties(Horizon PROPERTIES OUTPUT_NAME "Horizon")

# Include directories
target_include_directories(Horizon PRIVATE src)

# Source files
file(GLOB_RECURSE SRC_FILES "src/**.h" "src/**.cpp")
target_sources(Horizon PRIVATE ${SRC_FILES})

# Link Echo library
target_link_libraries(Horizon PRIVATE Echo)

# Enable solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find all asset and resource files
file(GLOB_RECURSE ASSET_FILES "${ASSETS_DIR}/**/*")
file(GLOB_RECURSE RESOURCE_FILES "${RESOURCE_DIR}/**/*")

# Preserve exact source folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SRC_FILES})

# Create proper virtual folder structure for assets and resources
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/assets" PREFIX "Assets" FILES ${ASSET_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Resources" PREFIX "Resources" FILES ${RESOURCE_FILES})

# Add assets and resources to the project
target_sources(Horizon PRIVATE ${ASSET_FILES} ${RESOURCE_FILES})

# Copy assets and resources to output directory
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${TARGET_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_DIR} ${TARGET_DIR}/Resources
)

# Add compile definitions based on build type
foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Horizon PRIVATE $<$<CONFIG:${CONFIG}>:EC_${CONFIG}> )
endforeach()

# Set output directories
set_target_properties(Horizon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Horizon
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/Horizon
)

# Copy slang after build
add_custom_command(
    TARGET Horizon
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:slang>   
        $<TARGET_FILE_DIR:Horizon>  
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/Config.h"
)
target_include_directories(Horizon PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)