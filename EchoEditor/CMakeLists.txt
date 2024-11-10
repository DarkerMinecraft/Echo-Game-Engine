project("EchoEditor" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(EchoEditor)

if (MSVC)
    set_property(TARGET EchoEditor PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(EchoEditor PRIVATE ${SOURCE_FILES})

target_include_directories(EchoEditor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/Echo/src"
    "${CMAKE_SOURCE_DIR}/Echo/vendor/spdlog/include"
    "${IncludeDir_glm}"
    "${IncludeDir_entt}"
    "${IncludeDir_ImGui}"
)

target_link_libraries(EchoEditor PRIVATE Echo)

target_compile_definitions(EchoEditor PRIVATE
    $<$<CONFIG:Debug>:ECHO_DEBUG>
    $<$<CONFIG:Release>:ECHO_RELEASE>
    $<$<CONFIG:Dist>:ECHO_DIST>
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Dist")
    target_compile_options(EchoEditor PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/O2>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3>
    )
endif()
