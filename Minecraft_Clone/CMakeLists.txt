cmake_minimum_required(VERSION 3.31.3)
project(Minecraft_Clone)
set(CMAKE_CXX_STANDARD 20)

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/Minecraft_Clone/assets)
set(TARGET_DIR $<TARGET_FILE_DIR:Minecraft_Clone>)

add_executable(Minecraft_Clone)
set_target_properties(Minecraft_Clone PROPERTIES OUTPUT_NAME "Minecraft Clone")

target_include_directories(Minecraft_Clone PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/Echo/src
    ${CMAKE_SOURCE_DIR}/Echo/vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/glm
    ${CMAKE_SOURCE_DIR}/Echo/vendor/entt/include
    ${CMAKE_SOURCE_DIR}/Echo/vendor/imgui
)

file(GLOB_RECURSE MINECRAFT_SRC_FILES
    "src/**.h"
    "src/**.cpp"
)

target_sources(Minecraft_Clone PRIVATE ${MINECRAFT_SRC_FILES})

target_link_libraries(Minecraft_Clone PRIVATE 
    Echo
)

foreach(CONFIG Debug Release Dist)
    target_compile_definitions(Minecraft_Clone PRIVATE $<$<CONFIG:${CONFIG}>:EC_${CONFIG}> )
endforeach()

set_target_properties(Minecraft_Clone PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Minecraft_Clone
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/Minecraft_Clone
)

add_custom_command(TARGET Minecraft_Clone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:Minecraft_Clone> $<TARGET_RUNTIME_DLLS:Minecraft_Clone>
    COMMAND_EXPAND_LISTS
)

file(GLOB_RECURSE ASSET_FILES "${ASSETS_DIR}/*")

add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSETS_DIR} ${TARGET_DIR}/assets
    DEPENDS ${ASSET_FILES}
    COMMENT "Copying assets to target directory"
)

add_dependencies(Minecraft_Clone CopyAssets)
