#pragma once

#include "CommandBuffer.h"

namespace Echo 
{

	class Window;

	enum class DeviceType
	{
		Vulkan
	};

	class Device 
	{
	public:
		virtual ~Device() = default;
		const static int MAX_FRAMES_IN_FLIGHT = 3;

		virtual DeviceType GetDeviceType() const = 0;

		static Scope<Device> Create(DeviceType type, Window* window, unsigned int width, unsigned int height);
	};

}