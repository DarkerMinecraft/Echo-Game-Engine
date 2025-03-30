cmake_minimum_required(VERSION 3.31.3)
project(Horizon)
set(CMAKE_CXX_STANDARD 20)

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/Horizon/assets)
set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/Horizon/Resources)
set(TARGET_DIR $<TARGET_FILE_DIR:Horizon>)

add_executable(Horizon)
set_target_properties(Horizon PROPERTIES OUTPUT_NAME "Horizon")

target_include_directories(Horizon PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/Echo/src
    ${CMAKE_SOURCE_DIR}/Echo/vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/glm
    ${CMAKE_SOURCE_DIR}/Echo/vendor/entt/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/imgui
    ${CMAKE_SOURCE_DIR}/Echo/vendor/ImGuizmo
    ${CMAKE_SOURCE_DIR}/Echo/vendor/RenderDocs/renderdoc/api/app
)

file(GLOB_RECURSE SRC_FILES
    "src/**.h"
    "src/**.cpp"
)

source_group(TREE "${CMAKE_SOURCE_DIR}/Horizon/src" FILES ${SRC_FILES})
target_sources(Horizon PRIVATE ${SRC_FILES})

target_link_libraries(Horizon PRIVATE Echo)

file(GLOB_RECURSE ASSET_FILES "${ASSETS_DIR}/**/*")
file(GLOB_RECURSE RESOURCE_FILES "${RESOURCE_DIR}/**/*")

source_group(TREE "${CMAKE_SOURCE_DIR}/Horizon/assets" FILES ${ASSET_FILES})
source_group(TREE "${CMAKE_SOURCE_DIR}/Horizon/Resources" FILES ${RESOURCE_FILES})

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${TARGET_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_DIR} ${TARGET_DIR}/Resources
)

target_sources(Horizon PRIVATE ${ASSET_FILES})

foreach(ASSET_FILE ${ASSET_FILES})
    file(RELATIVE_PATH RELATIVE_PATH "${ASSETS_DIR}" "${ASSET_FILE}")
    get_filename_component(RELATIVE_DIR "${RELATIVE_PATH}" DIRECTORY)
    if(RELATIVE_DIR)
        source_group("Assets\\${RELATIVE_DIR}" FILES "${ASSET_FILE}")
    else()
        source_group("Assets" FILES "${ASSET_FILE}")
    endif()
endforeach()

foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Horizon PRIVATE $<$<CONFIG:${CONFIG}>:EC_${CONFIG}> )
endforeach()

set_target_properties(Horizon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Horizon
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/Horizon
)

add_custom_command(
    TARGET Horizon
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:slang>   
        $<TARGET_FILE_DIR:Horizon>  
)
