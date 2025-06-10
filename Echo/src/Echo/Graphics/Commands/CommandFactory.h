#pragma once

#include "ICommand.h"

#include "Core/Application.h"
#include "Graphics/Primitives/Device.h"
#include "Graphics/Primitives/Framebuffer.h"
#include "Graphics/Primitives/Pipeline.h"
#include "Graphics/Primitives/Buffer.h"

#include <glm/glm.hpp>

namespace Echo
{
	class CommandFactory
	{
	public:
		static Ref<ICommand> ClearColorCommand(Ref<Framebuffer> framebuffer, uint32_t index, const glm::vec4& clearValues);
		static Ref<ICommand> BindPipelineCommand(Ref<Pipeline> pipeline);
		static Ref<ICommand> BindPipelineCommand(Pipeline* pipeline);

		static Ref<ICommand> DispatchCommand(float x, float y, float z);

		static Ref<ICommand> BindVertexBufferCommand(Ref<VertexBuffer> vertexBuffer);
		static Ref<ICommand> BindIndicesBufferCommand(Ref<IndexBuffer> indexBuffer);

		static Ref<ICommand> DrawCommand(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		static Ref<ICommand> DrawIndexedCommand(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
		static Ref<ICommand> DrawIndirectIndexed(Ref<IndirectBuffer> indirectBuffer, uint32_t offset, uint32_t drawCount, uint32_t stride);

		static Ref<ICommand> SetScissorCommand(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static Ref<ICommand> SetLineWidthCommand(float lineWidth);

		static Ref<ICommand> BeginRenderingCommand(Ref<Framebuffer> framebuffer);
		static Ref<ICommand> BeginRenderingCommand();
		static Ref<ICommand> EndRenderingCommand();

		static Ref<ICommand> RenderImGuiCommand();
	private:
		static inline DeviceType GetDeviceType() { return Application::Get().GetWindow().GetDevice()->GetDeviceType(); }
	};

}