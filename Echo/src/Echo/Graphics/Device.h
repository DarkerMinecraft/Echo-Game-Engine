#pragma once

#include "Echo/Core/Base.h"

#include "Swapchain.h"
#include "CommandList.h"
#include "Synchronization.h"
#include "Pipeline.h"
#include "Model.h"
#include "Descriptors.h"

#include <glm/glm.hpp>
#include <deque>

#include <imgui.h>

namespace Echo
{

	struct DeletionQueue
	{
		std::deque<std::function<void()>> Deletors;

		void PushFunction(std::function<void()>&& function)
		{
			Deletors.push_back(function);
		}

		void Flush()
		{
			for (auto it = Deletors.rbegin(); it != Deletors.rend(); it++)
			{
				(*it)();
			}

			Deletors.clear();
		}
	};

	struct FrameData
	{
		Scope<CommandPool> Pool;
		Scope<CommandBuffer> Buffer;

		Scope<Semaphore> SwapchainSemaphore, RenderSemaphore;
		Scope<Fence> RenderFence;
		
		Scope<DescriptorAllocatorGrowable> FrameDescriptors;

		DeletionQueue DeletionQueue;
	};

	constexpr unsigned int FRAME_OVERLAP = 2;

	enum class GraphicsAPI 
	{
		Vulkan,
		DirectX12
	};

	class Device 
	{
	public:
		virtual ~Device() = default;

		virtual GraphicsAPI GetGraphicsAPI() = 0;
		virtual Swapchain* GetSwapchain() = 0;

		virtual FrameData& GetCurrentFrame() = 0;
		
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void DrawBackground() = 0;

		virtual size_t GetGPUDrawSize() = 0;

		virtual void AddModel(Ref<Pipeline> pipeline, Ref<Model> model) = 0;

		virtual void DrawGeometry() = 0;

		virtual void Start() = 0;
		virtual void End() = 0;
		virtual void Wait() = 0;

		static Scope<Device> Create(GraphicsAPI api, void* window);
	};

}