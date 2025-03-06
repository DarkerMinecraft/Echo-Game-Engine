#pragma once

#include "Echo/Core/Base.h"

namespace Echo 
{

	enum class DeviceType
	{
		Vulkan
	};

	class Device 
	{
	public:
		virtual ~Device() = default;

		virtual DeviceType GetDeviceType() const = 0;
		
		static Scope<Device> Create(DeviceType type, const void* window, unsigned int width, unsigned int height);
	};

}