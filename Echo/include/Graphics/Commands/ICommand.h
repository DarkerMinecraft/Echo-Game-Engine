#pragma once

#include "Graphics/CommandBuffer.h"

namespace Echo 
{

	enum ImageLayout
	{
		General,
		Undefined,
		ColorAttachment,
		DepthAttachment,
		TransferSrc,
		TransferDst,
		ShaderReadOnly,
	};

	struct Extent2D 
	{
		uint32_t Width;
		uint32_t Height;
	};

	class ICommand 
	{
	public:
		virtual ~ICommand() = default;

		virtual void Execute(CommandBuffer* cmd) = 0;
	};

}