#include "pch.h"
#include "VulkanMaterial.h"

#include "Graphics/Pipeline.h"

namespace Echo 
{

	VulkanMaterial::VulkanMaterial(Device* device, Ref<Pipeline> pipeline)
		: m_Device((VulkanDevice*) device), m_Pipeline((VulkanPipeline*) pipeline.get())
	{

	}

	void VulkanMaterial::Reload()
	{

	}

	void VulkanMaterial::Load()
	{

	}

	void VulkanMaterial::Destroy()
	{
		m_Pipeline->Destroy();
	}

	void VulkanMaterial::SetParam(const std::string& name, const MaterialParamValue& value)
	{

	}

	const MaterialParamValue& VulkanMaterial::GetParam(const std::string& name) const
	{
		MaterialParamValue val;
		return val;
	}

	bool VulkanMaterial::HasParam(const std::string& name) const
	{
		return false;
	}

}