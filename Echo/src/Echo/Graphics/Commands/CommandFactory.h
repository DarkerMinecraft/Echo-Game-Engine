#pragma once

#include "ICommand.h"

#include "Echo/Core/Application.h"
#include "Echo/Graphics/Device.h"
#include "Echo/Graphics/Image.h"
#include "Echo/Graphics/Pipeline.h"
#include "Echo/Graphics/Buffer.h"

#include <glm/glm.hpp>

namespace Echo
{
	class CommandFactory
	{
	public:
		static Ref<ICommand> TransitionImageCommand(Ref<Image> image, ImageLayout oldLayout, ImageLayout newLayout);
		static Ref<ICommand> CopyImageToImageCommand(Ref<Image> srcImage, Ref<Image> dstImage);

		static Ref<ICommand> ClearColorCommand(Ref<Image> image, const glm::vec4& clearValues);
		static Ref<ICommand> BindPipelineCommand(Ref<Pipeline> pipeline);
		static Ref<ICommand> DispatchCommand(float x, float y, float z);

		static Ref<ICommand> BindVertexBufferCommand(Ref<VertexBuffer> vertexBuffer);
		static Ref<ICommand> BindIndicesBufferCommand(Ref<IndexBuffer> indexBuffer);

		static Ref<ICommand> DrawCommand(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		static Ref<ICommand> DrawIndexedCommand(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
		static Ref<ICommand> DrawIndirectIndexed(Ref<IndirectBuffer> indirectBuffer, uint32_t offset, uint32_t drawCount, uint32_t stride);

		static Ref<ICommand> BeginRenderingCommand(Ref<Image> image);
		static Ref<ICommand> BeginRenderingCommand();
		static Ref<ICommand> EndRenderingCommand();

		static Ref<ICommand> RenderImGuiCommand();
	private:
		static inline DeviceType GetDeviceType() { return Application::Get().GetWindow().GetDevice()->GetDeviceType(); }
	};

}