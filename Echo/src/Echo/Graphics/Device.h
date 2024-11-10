#pragma once

#include "Echo/Core/Base.h"

#include "Resource.h"
#include "CommandList.h"

#include <memory>
#include <span>
#include <expected>

namespace Echo 
{
	class CommandList;
	class Swapchain;
	struct SwapchainWin32CreateInfo;

	enum class GraphicsAPI
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

	enum class QueueType
	{
		Graphics,
		Compute,
		Copy,
		VideoDecode,
		VideoEncode,
	};

	struct Fence
	{
		virtual Result GetStatus(uint64_t value) = 0;
		virtual Result WaitForValue(uint64_t value) = 0;
	};

	struct SubmitFenceInfo 
	{
		Fence* Fence;
		uint64_t Value;
	};

	struct SubmitInfo 
	{
		QueueType QueueType;
		Swapchain* WaitSwapchain;
		Swapchain* PresentSwapchain;
		std::span<SubmitFenceInfo> WaitInfos;
		std::span<CommandList*> CommandLists;
		std::span<SubmitFenceInfo> SignalInfos;
	};

	struct GraphicsDeviceCreateInfo 
	{
		GraphicsAPI GraphicsAPI;
		bool EnableValidation;
		bool EnableGPUValidation;
		bool EnableLocking;
	};

	class Device 
	{
	public:
		virtual ~Device() = default;
		Device(const Device& other) = delete;
		Device(Device&& other) = delete;
		Device& operator=(const Device& other) = delete;
		Device& operator=(Device&& other) = delete;

		virtual Result WaitIdle() = 0;
		virtual Result QueueWaitIdle(QueueType queue, uint64_t timeout) = 0;

		virtual const GraphicsAPI GetGraphicsAPI() const = 0;

		virtual Scope<Swapchain> CreateSwapchain(const SwapchainWin32CreateInfo& createInfo) = 0;
		//virtual Scope<CommandPool> CreateCommandPool(const CommandPoolCreateInfo& createInfo) = 0;

		virtual std::expected<Fence*, Result> CreateFence(uint64_t initalValue) = 0;
		virtual void DestroyFence(Fence* fence) = 0;

		static Scope<Device> Create(const GraphicsDeviceCreateInfo& createInfo);
	};
}