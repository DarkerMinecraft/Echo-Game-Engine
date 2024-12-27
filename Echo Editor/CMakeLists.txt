cmake_minimum_required(VERSION 3.31.3)
project(Echo_Editor)
set(CMAKE_CXX_STANDARD 20)

add_executable(Echo_Editor)
set_target_properties(Echo_Editor PROPERTIES OUTPUT_NAME "Echo Editor")

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

target_sources(Echo_Editor PRIVATE ${EDITOR_SRC_FILES})

target_link_libraries(Echo_Editor PRIVATE Echo)

foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Echo_Editor PRIVATE $<$<CONFIG:${CONFIG}>:EC_${CONFIG}> )
endforeach()

set_target_properties(Echo_Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Echo_Editor
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/Echo_Editor
)
