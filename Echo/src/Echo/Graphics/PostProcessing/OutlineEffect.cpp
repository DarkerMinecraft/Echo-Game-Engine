#include "pch.h"
#include "OutlineEffect.h"

namespace Echo
{
	void OutlineEffect::Init()
	{
		// Create shader for outline post-processing
		ShaderSpecification outlineShaderSpec;
		outlineShaderSpec.ShaderName = "Outline Post-Process";

		// Vertex shader for fullscreen quad
		outlineShaderSpec.VertexShaderSource = R"(
            struct VSInput
            {
                float3 position : ATTRIB0;
                float2 uv : ATTRIB1;
            };
            
            struct VSOutput
            {
                float4 position : SV_POSITION;
                float2 uv : TEXCOORD0;
            };
            
            [shader("vertex")]
            VSOutput main(VSInput input)
            {
                VSOutput output;
                output.position = float4(input.position, 1.0);
                output.uv = input.uv;
                return output;
            }
        )";

		// Fragment shader to detect edges and draw outlines
		outlineShaderSpec.FragmentShaderSource = R"(
            struct PSInput
            {
                float4 position : SV_POSITION;
                float2 uv : TEXCOORD0;
            };
            
            // Textures: color buffer and ID buffer
            [[vk::binding(0, 0)]] Texture2D<float4> colorBuffer;
            [[vk::binding(1, 0)]] Texture2D<int> idBuffer;
            [[vk::binding(2, 0)]] SamplerState texSampler;
            
            // Outline parameters
            [[vk::binding(3, 0)]] cbuffer OutlineParams : register(b0)
            {
                int selectedEntityID;
                float4 outlineColor;
                float outlineThickness;
                float2 texelSize; // 1/width, 1/height
            };
            
            [shader("pixel")]
            float4 main(PSInput input) : SV_TARGET
            {
                // Sample the color
                float4 color = colorBuffer.Sample(texSampler, input.uv);
                
                // If no entity is selected, just return the color
                if (selectedEntityID < 0)
                    return color;
                
                // Get the entity ID at this pixel
                int entityID = idBuffer.Load(int3(input.position.xy, 0));
                
                // If this pixel belongs to the selected entity, no outline needed
                if (entityID == selectedEntityID)
                    return color;
                
                // Check neighboring pixels for the selected entity
                bool isOutline = false;
                
                // Sample in a pattern around the current pixel
                for (int y = -1; y <= 1 && !isOutline; y++)
                {
                    for (int x = -1; x <= 1 && !isOutline; x++)
                    {
                        // Skip the center pixel
                        if (x == 0 && y == 0)
                            continue;
                            
                        // Calculate UV for the neighboring pixel
                        float2 offsetUV = input.uv + float2(x, y) * texelSize * outlineThickness;
                        
                        // Make sure we're still in the texture
                        if (offsetUV.x >= 0.0 && offsetUV.x <= 1.0 && 
                            offsetUV.y >= 0.0 && offsetUV.y <= 1.0)
                        {
                            // Sample the ID at this neighbor
                            int neighborID = idBuffer.Sample(texSampler, offsetUV);
                            
                            // If this neighbor is the selected entity, we're at the outline
                            if (neighborID == selectedEntityID)
                            {
                                isOutline = true;
                                break;
                            }
                        }
                    }
                }
                
                // Apply outline if needed
                return isOutline ? outlineColor : color;
            }
        )";

		m_Shader = Shader::Create(outlineShaderSpec);

		// Create pipeline for the outline effect
		PipelineSpecification pipelineSpec;
		pipelineSpec.EnableBlending = false;
		pipelineSpec.EnableDepthTest = false;
		pipelineSpec.EnableDepthWrite = false;
		pipelineSpec.EnableCulling = false;
		pipelineSpec.FillMode = Fill::Solid;

		// Setup the vertex layout for a fullscreen quad
		pipelineSpec.VertexLayout = {
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float2, "TexCoord" }
		};

		// Setup descriptor sets for the textures and uniform buffer
		pipelineSpec.DescriptionSetLayouts = {
			{ 0, DescriptorType::SampledImage, 1, ShaderStage::Fragment }, // Color buffer
			{ 1, DescriptorType::SampledImage, 1, ShaderStage::Fragment }, // ID buffer
			{ 2, DescriptorType::SampledImage, 1, ShaderStage::Fragment }, // Sampler
			{ 3, DescriptorType::UniformBuffer, 1, ShaderStage::Fragment } // Outline params
		};

		m_Pipeline = Pipeline::Create(m_Shader, pipelineSpec);

		// Create uniform buffer for outline parameters
		struct OutlineParams
		{
			int SelectedEntityID = -1;
			glm::vec4 OutlineColor = { 1.0f, 0.35f, 0.0f, 1.0f };
			float OutlineThickness = 1.0f;
			glm::vec2 TexelSize = { 1.0f / 1280.0f, 1.0f / 720.0f };
		};

		OutlineParams initialParams;
		m_UniformBuffer = UniformBuffer::Create(&initialParams, sizeof(OutlineParams));
	}

	void OutlineEffect::Cleanup()
	{
		m_Shader.reset();
		m_Pipeline.reset();
		m_UniformBuffer.reset();
	}

	void OutlineEffect::Apply(CommandList& cmd, Ref<Framebuffer> source, Ref<Framebuffer> destination)
	{
		// Skip if no entity is selected
		if (m_SelectedEntityID < 0)
		{
			// Just copy the source to destination without processing
			// For simplicity, we'll still run the shader but with an invalid ID
		}

		// Update uniform buffer with current settings
		struct OutlineParams
		{
			int SelectedEntityID;
			glm::vec4 OutlineColor;
			float OutlineThickness;
			glm::vec2 TexelSize;
		};

		OutlineParams params;
		params.SelectedEntityID = m_SelectedEntityID;
		params.OutlineColor = m_OutlineColor;
		params.OutlineThickness = m_OutlineThickness;
		params.TexelSize = { 1.0f / source->GetWidth(), 1.0f / source->GetHeight() };

		m_UniformBuffer->SetData(&params, sizeof(OutlineParams));

		// Set the source framebuffer
		cmd.SetSourceFramebuffer(source);

		// Begin rendering
		cmd.Begin();
		cmd.BeginRendering(destination);

		// Bind the pipeline and resources
		cmd.BindPipeline(m_Pipeline);
		cmd.BindVertexBuffer(PostProcessingSystem::GetFullscreenQuadVertexBuffer());
		cmd.BindIndicesBuffer(PostProcessingSystem::GetFullscreenQuadIndexBuffer());

		// Bind textures and uniform buffer
		m_Pipeline->WriteDescriptorCombinedImage(source, 0, 0); // Color buffer
		m_Pipeline->WriteDescriptorCombinedImage(source, 1, 1); // ID buffer
		m_Pipeline->WriteDescriptorUniformBuffer(m_UniformBuffer, 3);

		// Draw the fullscreen quad
		cmd.DrawIndexed(6, 1, 0, 0, 0);

		// End rendering
		cmd.EndRendering();
		cmd.Execute();
	}
}