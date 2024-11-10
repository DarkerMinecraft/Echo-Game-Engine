# Define the toolchain for different operating systems
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring for Windows")
    
    # Set compiler paths for Windows (example with Visual Studio)
    set(CMAKE_C_COMPILER "cl.exe")
    set(CMAKE_CXX_COMPILER "cl.exe")

    # Set Windows-specific options
    set(CMAKE_GENERATOR_PLATFORM "x64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_WINDOWS")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring for Linux")

    # Set compiler paths for Linux
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")

    # Set Linux-specific options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX -fPIC")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring for macOS")

    # Set compiler paths for macOS
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

    # Set macOS-specific options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_MACOS -stdlib=libc++")
    set(CMAKE_MACOSX_RPATH ON)

else()
    message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Additional common settings for all platforms
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
