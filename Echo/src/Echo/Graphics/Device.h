#pragma once

#include "CommandBuffer.h"
#include <functional>
#include <memory>

namespace Echo 
{

	class Window;

	enum class DeviceType
	{
		Vulkan
	};

	enum class DeviceState
	{
		Uninitialized,
		Initializing,
		Ready,
		Failed
	};

	class Device 
	{
	public:
		virtual ~Device() = default;
		const static int MAX_FRAMES_IN_FLIGHT = 3;

		virtual const DeviceType GetDeviceType() const = 0;
		virtual const DeviceState GetState() const = 0;

		// Lazy initialization methods
		virtual bool IsInitialized() const = 0;
		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;

		// Force initialization if needed
		virtual bool EnsureInitialized() = 0;

		//Render Caps Methods
		virtual const uint32_t GetMaxTextureSlots() const = 0;

		static Scope<Device> Create(DeviceType type, Window* window, unsigned int width, unsigned int height);

		// Callback for initialization state changes
		using StateChangeCallback = std::function<void(DeviceState oldState, DeviceState newState)>;
		virtual void SetStateChangeCallback(StateChangeCallback callback) = 0;
	};

}