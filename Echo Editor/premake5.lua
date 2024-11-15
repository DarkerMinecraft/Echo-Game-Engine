project "Echo Editor"
    location ""
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    staticruntime "off"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "src/**.h",
        "src/**.cpp"
    }
    
    includedirs
    {
        "src/",
        "%{wks.location}/Echo/src",
        "%{wks.location}/Echo/vendor/spdlog/include",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGui}",
    }

    links
    {
        "Echo"
    }

    filter "configurations:Debug"
        defines "EC_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines "EC_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "EC_DIST"
        runtime "Release"
        optimize "on"