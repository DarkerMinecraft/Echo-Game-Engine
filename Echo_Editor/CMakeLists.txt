cmake_minimum_required(VERSION 3.31.3)
project(Echo_Editor)
set(CMAKE_CXX_STANDARD 20)

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/Echo_Editor/assets)
set(TARGET_DIR $<TARGET_FILE_DIR:Echo_Editor>)

add_executable(Echo_Editor)
set_target_properties(Echo_Editor PROPERTIES OUTPUT_NAME "Echo Editor")

find_package(Python3 COMPONENTS Interpreter REQUIRED)

target_include_directories(Echo_Editor PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/Echo/src
    ${CMAKE_SOURCE_DIR}/Echo/vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/glm
    ${CMAKE_SOURCE_DIR}/Echo/vendor/entt/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/imgui
)

file(GLOB_RECURSE EDITOR_SRC_FILES
    "src/**.h"
    "src/**.cpp"
)

source_group(TREE "${CMAKE_SOURCE_DIR}/Echo_Editor/src" FILES ${SRC_FILES})

target_sources(Echo_Editor PRIVATE ${EDITOR_SRC_FILES})

target_link_libraries(Echo_Editor PRIVATE Echo)

file(GLOB_RECURSE ASSET_FILES "${ASSETS_DIR}/*")

file(GLOB HLSL_FILES "${ASSET_FILES}/shaders/*.hlsl")

source_group(TREE "${CMAKE_SOURCE_DIR}/Echo_Editor/assets" FILES ${ASSET_FILES})

foreach(shader ${HLSL_FILES})
    set_source_files_properties(${shader} PROPERTIES LANGUAGE HLSL)
endforeach()


add_custom_target(EditorCopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${TARGET_DIR}/assets
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/CMake/ConvertHLSLFiles.py ${TARGET_DIR}/assets
    DEPENDS ${ASSET_FILES}
    COMMENT "Copying assets and converting .hlsl files to UTF-16"
)

add_dependencies(Echo_Editor EditorCopyAssets)

target_sources(Echo_Editor PRIVATE ${ASSET_FILES})

source_group("Assets" FILES ${ASSET_FILES})

foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Echo_Editor PRIVATE $<$<CONFIG:${CONFIG}>:EC_${CONFIG}> )
endforeach()

set_target_properties(Echo_Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Echo_Editor
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/Echo_Editor
)
