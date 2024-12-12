VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["glm"] = "%{wks.location}/Echo/vendor/glm"
IncludeDir["GLFW"] = "%{wks.location}/Echo/vendor/GLFW/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/include"
IncludeDir["spdlog"] = "%{wks.location}/Echo/vendor/spdlog/include"
IncludeDir["DirectX"] = "%{wks.location}/Echo/vendor/DirectX/include"
IncludeDir["entt"] = "%{wks.location}/Echo/vendor/entt/include"
IncludeDir["ImGui"] = "%{wks.location}/Echo/vendor/imgui"
IncludeDir["vkbootstrap"] = "%{wks.location}/Echo/vendor/vk-bootstrap/src"
IncludeDir["VMA"] = "%{wks.location}/Echo/vendor/VulkanMemoryAllocator/include"
IncludeDir["tinyobjloader"] = "%{wks.location}/Echo/vendor/tinyobjloader"
IncludeDir["stb"] = "%{wks.location}/Echo/vendor/stb"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"