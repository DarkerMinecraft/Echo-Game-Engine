cmake_minimum_required(VERSION 3.31.3)
project(Bloomwood)
set(CMAKE_CXX_STANDARD 20)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(TARGET_DIR $<TARGET_FILE_DIR:Bloomwood>)

# Create the executable
add_executable(Bloomwood)
set_target_properties(Bloomwood PROPERTIES OUTPUT_NAME "Bloomwood")

# Include directories
target_include_directories(Bloomwood PRIVATE 
    src
    ${CMAKE_SOURCE_DIR}/Echo/src/Echo
)

# Source files
file(GLOB_RECURSE SRC_FILES "src/**.h" "src/**.cpp")
target_sources(Bloomwood PRIVATE ${SRC_FILES})

# Link Echo library
target_link_libraries(Bloomwood PRIVATE Echo)

# Enable solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find all asset and resource files
file(GLOB_RECURSE ASSET_FILES "${ASSETS_DIR}/**/*")
file(GLOB_RECURSE RESOURCE_FILES "${RESOURCE_DIR}/**/*")

# Preserve exact source folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SRC_FILES})

# Create proper virtual folder structure for assets and resources
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/assets" PREFIX "Assets" FILES ${ASSET_FILES})

# Add assets and resources to the project
target_sources(Bloomwood PRIVATE ${ASSET_FILES})

# Copy assets and resources to output directory
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${TARGET_DIR}/assets
)

# Add compile definitions based on build type
foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Bloomwood PRIVATE $<$<CONFIG:${CONFIG}>:EC_${CONFIG}> )
endforeach()

# Set output directories
set_target_properties(Bloomwood PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Bloomwood
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/Bloomwood
)

# Copy slang after build
add_custom_command(
    TARGET Bloomwood
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:slang>   
        $<TARGET_FILE_DIR:Bloomwood>  
)