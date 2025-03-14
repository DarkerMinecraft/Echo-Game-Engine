#pragma once 

#include "Echo/Core/Base.h"

#include "Commands/ICommand.h"
#include "Commands/CommandFactory.h"
#include "CommandBuffer.h"

#include "Image.h"

#include <vector>

namespace Echo
{

	class CommandList 
	{
	public:
		CommandList(); 
		virtual ~CommandList() = default;
		
		void Begin();

		void TransitionImage(Ref<Image> img, ImageLayout newLayout, ImageLayout oldLayout) { RecordCommand(CommandFactory::TransitionImageCommand(img, newLayout, oldLayout)); }
		void CopyImageToImage(Ref<Image> srcImage, Ref<Image> dstImage) { RecordCommand(CommandFactory::CopyImageToImageCommand(srcImage, dstImage)); }
		void ClearColor(Ref<Image> img, const glm::vec4& clearValues) { RecordCommand(CommandFactory::ClearColorCommand(img, clearValues)); }
		void Dispatch(float x, float y, float z) { RecordCommand(CommandFactory::DispatchCommand(x, y, z)); }

		void BindPipeline(Ref<Pipeline> pipeline) { RecordCommand(CommandFactory::BindPipelineCommand(pipeline)); }
		void BindVertexBuffer(Ref<VertexBuffer> vertexBuffer) { RecordCommand(CommandFactory::BindVertexBufferCommand(vertexBuffer)); }
		void BindIndicesBuffer(Ref<IndexBuffer> indexBuffer) { RecordCommand(CommandFactory::BindIndicesBufferCommand(indexBuffer)); }

		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) { RecordCommand(CommandFactory::DrawCommand(vertexCount, instanceCount, firstVertex, firstInstance)); }
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) { RecordCommand(CommandFactory::DrawIndexedCommand(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance)); }
		void DrawIndirectIndexed(Ref<IndirectBuffer> indirectBuffer, uint32_t offset, uint32_t drawCount, uint32_t stride) { RecordCommand(CommandFactory::DrawIndirectIndexed(indirectBuffer, offset, drawCount, stride)); }

		void BeginRendering(Ref<Image> image) { RecordCommand(CommandFactory::BeginRenderingCommand(image)); }
		void BeginRendering() { RecordCommand(CommandFactory::BeginRenderingCommand()); }
		void EndRendering() { RecordCommand(CommandFactory::EndRenderingCommand()); }

		void RenderImGui() { RecordCommand(CommandFactory::RenderImGuiCommand()); }

		void SetSrcImage(Ref<Image> srcImage) { m_CommandBuffer->SetSourceImage(srcImage); }
		void SetShouldPresent(bool shouldPresent) { m_CommandBuffer->SetShouldPresent(shouldPresent); }
		void SetDrawToSwapchain(bool drawToSwapchain) { m_CommandBuffer->SetDrawToSwapchain(drawToSwapchain); }
		
		void Execute();
	private:
		void RecordCommand(Ref<ICommand> command);
	private:
		std::vector<Ref<ICommand>> m_Commands;
		Ref<CommandBuffer> m_CommandBuffer;
	};

}