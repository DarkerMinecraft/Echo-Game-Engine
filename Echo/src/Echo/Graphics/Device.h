#pragma once

#include "Echo/Core/Base.h"

#include "Resource.h"
#include "CommandList.h"
#include "Swapchain.h"

#include <memory>
#include <span>
#include <expected>

namespace Echo 
{

	enum class API
	{
		DirectX, Vulkan
	};

	enum class Result
	{
		Sucess,
		Wait_Timeout,
		Error_Wait_Failed,
		Error_Out_Of_Memory,
		Error_Invalid_Parameters,
		Error_Device_Lost,
		Error_No_Resource,
		Error_Unknown
	};

	struct GraphicsDeviceCreateInfo 
	{
		API GraphicsAPI;
		bool EnableValidation;
		bool EnableGPUValidation;
		bool EnableLocking;
	};

	class Device 
	{
	public:
		Device() = default;
		virtual ~Device() = default;
		Device(const Device& other) = delete;
		Device(Device&& other) = delete;
		Device& operator=(const Device& other) = delete;
		Device& operator=(Device&& other) = delete;

		virtual const API GetGraphicsAPI() const = 0;

		virtual Swapchain* GetSwapchain() = 0;
		virtual CommandBuffer* GetCommandBuffer() = 0;

		static Scope<Device> Create(void* hwnd, const GraphicsDeviceCreateInfo& createInfo);
	};
}