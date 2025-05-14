#pragma once

#include "Graphics/Material.h"
#include "Graphics/Device.h"

#include "VulkanDevice.h"

#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanPipeline.h"

namespace Echo 
{

	class VulkanMaterial : public Material 
	{
	public:
		VulkanMaterial(Device* device, Ref<Pipeline> pipeline);
		
		virtual void Reload() override;
		virtual void Load() override;
		virtual void Destroy() override;

		virtual void SetParam(const std::string& name, const MaterialParamValue& value) override;
		virtual const MaterialParamValue& GetParam(const std::string& name) const override;
		virtual bool HasParam(const std::string& name) const override;
	private:
		VulkanDevice* m_Device;
	
		VulkanPipeline* m_Pipeline;
	};


}