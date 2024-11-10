set(VULKAN_SDK $ENV{VULKAN_SDK})

if(NOT VULKAN_SDK)
    message(FATAL_ERROR "VULKAN_SDK environment variable not set")
endif()

set(IncludeDir
    glm         "${CMAKE_SOURCE_DIR}/Echo/vendor/glm"
    GLFW        "${CMAKE_SOURCE_DIR}/Echo/vendor/GLFW/include"
    VulkanSDK   "${VULKAN_SDK}/include"
    spdlog      "${CMAKE_SOURCE_DIR}/Echo/vendor/spdlog/include"
    DirectX     "${CMAKE_SOURCE_DIR}/Echo/vendor/DirectX/include"
    ImGui       "${CMAKE_SOURCE_DIR}/Echo/vendor/imgui"
    entt        "${CMAKE_SOURCE_DIR}/Echo/vendor/entt/include"
)

set(LibraryDir
    VulkanSDK   "${VULKAN_SDK}/Lib"
)

set(Library
    Vulkan                      "${LibraryDir_VulkanSDK}/vulkan-1.lib"
    VulkanUtils                 "${LibraryDir_VulkanSDK}/VkLayer_utils.lib"

    ShaderC_Debug               "${LibraryDir_VulkanSDK}/shaderc_sharedd.lib"
    SPIRV_Cross_Debug           "${LibraryDir_VulkanSDK}/spirv-cross-cored.lib"
    SPIRV_Cross_GLSL_Debug      "${LibraryDir_VulkanSDK}/spirv-cross-glsld.lib"
    SPIRV_Tools_Debug           "${LibraryDir_VulkanSDK}/SPIRV-Toolsd.lib"

    ShaderC_Release             "${LibraryDir_VulkanSDK}/shaderc_shared.lib"
    SPIRV_Cross_Release         "${LibraryDir_VulkanSDK}/spirv-cross-core.lib"
    SPIRV_Cross_GLSL_Release    "${LibraryDir_VulkanSDK}/spirv-cross-glsl.lib"
)
