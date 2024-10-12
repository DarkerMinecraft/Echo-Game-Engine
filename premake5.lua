workspace "Echo"
architecture "x64"
systemversion "latest"
configurations { "Debug", "Release", "Dist" }

startproject "Echo Editor"

include "Dependencies.lua"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "Echo/vendor/GLFW"
group ""

project "Echo"
    location "Echo"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "Echo/src/pch.cpp"

    nuget { "directxtk12_desktop_2019:2024.6.5.1" }

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/**.hpp",
        "%{prj.name}/vendor/glm/**.inl"
    }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
        "GLM_FORCE_CTOR_INIT"
	}

    includedirs
    {
        "%{prj.name}/src",
        "%%{IncludeDir.spdlog}",
        "%{IncludeDir.DirectX}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.VulkanSDK}"
    }

    libdirs 
    {
        "%{LibraryDir.VulkanSDK}"
    }

    links 
    {
        "GLFW",
        "vulkan-1",
    }

    filter "system:windows" 
        defines "USING_DIRECTX_HEADERS"

    filter "configurations:Debug"
		defines "ECHO_DEBUG"
		runtime "Debug"
		symbols "on"

        links
		{
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "ECHO_RELEASE"
		runtime "Release"
		optimize "on"

        links
		{
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

	filter "configurations:Dist"
		defines "ECHO_DIST"
		runtime "Release"
		optimize "on"

        links
		{
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

project "Echo Editor"
    location "Echo Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }
    
    includedirs
    {
        "%{prj.name}/src",
        "Echo/src",
        "Echo/vendor/spdlog/include",
        "%{IncludeDir.glm}",
    }

    links
    {
        "Echo"
    }

    filter {}
        removefiles { "Echo/src/Platform/**" }
    
    filter "configurations:Debug"
        defines "ECHO_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines "ECHO_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "ECHO_DIST"
        runtime "Release"
        optimize "on"