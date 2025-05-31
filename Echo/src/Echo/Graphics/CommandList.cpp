#include "pch.h"
#include "CommandList.h"
#include "Utils/DeferredInitManager.h"

namespace Echo 
{

	CommandList::CommandList()
		: m_CommandBuffer(CommandBuffer::Create())
	{

	}

	void CommandList::Begin() 
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[CommandList] Device not ready, deferring Begin");
			Echo::DeferredInitManager::Enqueue([this]() {
				EC_CORE_INFO("[CommandList] Running deferred Begin (device now ready)");
				m_Commands.clear();
				m_CommandBuffer->Start();
			});
			return;
		}
		m_Commands.clear();
		m_CommandBuffer->Start();
	}

	void CommandList::RecordCommand(Ref<ICommand> command) 
	{
		m_Commands.push_back(command);
	}

	void CommandList::Execute(bool isLastPass)
	{
		for (Ref<ICommand> command : m_Commands)
		{
			command->Execute(m_CommandBuffer.get());
		}

		m_CommandBuffer->End();
		m_CommandBuffer->Submit(isLastPass);
	}

	void CommandList::SetSourceFramebuffer(Ref<Framebuffer> framebuffer) {
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[CommandList] Device not ready, deferring SetSourceFramebuffer");
			Echo::DeferredInitManager::Enqueue([this, framebuffer]() {
				EC_CORE_INFO("[CommandList] Running deferred SetSourceFramebuffer (device now ready)");
				m_CommandBuffer->SetSourceFramebuffer(framebuffer);
			});
			return;
		}
		m_CommandBuffer->SetSourceFramebuffer(framebuffer);
	}

	void CommandList::SetShouldPresent(bool shouldPresent) {
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[CommandList] Device not ready, deferring SetShouldPresent");
			Echo::DeferredInitManager::Enqueue([this, shouldPresent]() {
				EC_CORE_INFO("[CommandList] Running deferred SetShouldPresent (device now ready)");
				m_CommandBuffer->SetShouldPresent(shouldPresent);
			});
			return;
		}
		m_CommandBuffer->SetShouldPresent(shouldPresent);
	}

	void CommandList::SetDrawToSwapchain(bool drawToSwapchain) {
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[CommandList] Device not ready, deferring SetDrawToSwapchain");
			Echo::DeferredInitManager::Enqueue([this, drawToSwapchain]() {
				EC_CORE_INFO("[CommandList] Running deferred SetDrawToSwapchain (device now ready)");
				m_CommandBuffer->SetDrawToSwapchain(drawToSwapchain);
			});
			return;
		}
		m_CommandBuffer->SetDrawToSwapchain(drawToSwapchain);
	}

	void CommandList::End() {
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[CommandList] Device not ready, deferring End");
			Echo::DeferredInitManager::Enqueue([this]() {
				EC_CORE_INFO("[CommandList] Running deferred End (device now ready)");
				m_CommandBuffer->End();
			});
			return;
		}
		m_CommandBuffer->End();
	}

}