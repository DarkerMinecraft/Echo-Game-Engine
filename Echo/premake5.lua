project "Echo"
    location ""
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    nuget { "directxtk12_desktop_2019:2024.6.5.1" }

    files
    {
        "src/**.h",
        "src/**.cpp",
        "vendor/glm/**.hpp",
        "vendor/glm/**.inl"
    }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
        "GLM_FORCE_CTOR_INIT",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}

    includedirs
    {
        "src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.DirectX}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.vkbootstrap}",
        "%{IncludeDir.VMA}",
        "%{IncludeDir.tinyobjloader}",
        "%{IncludeDir.stb}"
    }

    libdirs 
    {
        "%{LibraryDir.VulkanSDK}"
    }

    links 
    {
        "GLFW",
        "ImGui",
        "%{Library.Vulkan}",
        "vk-bootstrap"
    }

    filter "system:windows" 
        defines "USING_DIRECTX_HEADERS"

    filter "configurations:Debug"
		defines "ECHO_DEBUG"
		runtime "Debug"
		symbols "on"

        links
		{
            "%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "ECHO_RELEASE"
		runtime "Release"
		optimize "on"

        links
		{
            "%{Library.ShaderC_Release}",
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
