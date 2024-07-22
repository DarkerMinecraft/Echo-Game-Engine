workspace "Echo"
architecture "x64"
systemversion "latest"
configurations { "Debug", "Release", "Dist" }

startproject "Echo Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include"
    }

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
    }

    links
    {
        "Echo"
    }

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