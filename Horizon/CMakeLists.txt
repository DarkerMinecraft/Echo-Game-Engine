cmake_minimum_required(VERSION 3.31.3)
project(Horizon)
set(CMAKE_CXX_STANDARD 20)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(TARGET_DIR $<TARGET_FILE_DIR:Horizon>)

# Create the executable
add_executable(Horizon)
set_target_properties(Horizon PROPERTIES OUTPUT_NAME "Horizon")

# Find all source files
file(GLOB_RECURSE SRC_FILES "src/**.h" "src/**.cpp")
file(GLOB_RECURSE ASSET_FILES "${ASSETS_DIR}/**/*")

# Preserve exact source folder structure in the IDE
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SRC_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/assets" PREFIX "Assets" FILES ${ASSET_FILES})

# Include directories
target_include_directories(Horizon PRIVATE 
    src
    ${CMAKE_SOURCE_DIR}/Echo/src/Echo
)

# Source files
target_sources(Horizon PRIVATE ${SRC_FILES} ${ASSET_FILES} ${RESOURCE_FILES})

# Link Echo library
target_link_libraries(Horizon PRIVATE Echo)

# Add compile definitions based on build type
foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Horizon PRIVATE $<$<CONFIG:${CONFIG}>:EC_${CONFIG}> )
endforeach()

# Set output directories
set_target_properties(Horizon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Horizon
)

# Copy assets and resources to output directory
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${TARGET_DIR}/assets
)

# Copy slang after build
add_custom_command(
    TARGET Horizon
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:slang>   
        $<TARGET_FILE_DIR:Horizon>  
)