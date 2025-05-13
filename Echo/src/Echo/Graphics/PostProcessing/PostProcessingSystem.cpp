#include "pch.h"
#include "PostProcessingSystem.h"

namespace Echo
{
	PostProcessingSystem::PostProcessingSystemData PostProcessingSystem::s_Data;

	void PostProcessingSystem::Init()
	{
		// Create fullscreen quad
		float quadVertices[] = {
			// Position (x,y,z)    // TexCoord (u,v)
			-1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f,    0.0f, 1.0f
		};

		uint32_t quadIndices[] = {
			0, 1, 2,
			2, 3, 0
		};

		s_Data.FullscreenQuadVertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		s_Data.FullscreenQuadIndexBuffer = IndexBuffer::Create(quadIndices, 6);

		// Create intermediate framebuffer (initialize with default size, will be resized as needed)
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		s_Data.IntermediateFramebuffer = Framebuffer::Create(fbSpec);
	}

	void PostProcessingSystem::Shutdown()
	{
		// Clean up all effects
		for (auto& [name, effect] : s_Data.Effects)
		{
			effect->Cleanup();
		}

		s_Data.Effects.clear();
		s_Data.EffectOrder.clear();

		s_Data.FullscreenQuadVertexBuffer.reset();
		s_Data.FullscreenQuadIndexBuffer.reset();
		s_Data.IntermediateFramebuffer.reset();
	}

	void PostProcessingSystem::AddEffect(const Ref<PostProcessingEffect>& effect)
	{
		const std::string& name = effect->GetName();

		// Initialize the effect
		effect->Init();

		// Store the effect
		s_Data.Effects[name] = effect;

		// Add to order list if not already present
		if (std::find(s_Data.EffectOrder.begin(), s_Data.EffectOrder.end(), name) == s_Data.EffectOrder.end())
		{
			s_Data.EffectOrder.push_back(name);
		}
	}

	void PostProcessingSystem::RemoveEffect(const std::string& effectName)
	{
		auto it = s_Data.Effects.find(effectName);
		if (it != s_Data.Effects.end())
		{
			// Clean up the effect
			it->second->Cleanup();

			// Remove from effects map
			s_Data.Effects.erase(it);

			// Remove from order list
			auto orderIt = std::find(s_Data.EffectOrder.begin(), s_Data.EffectOrder.end(), effectName);
			if (orderIt != s_Data.EffectOrder.end())
			{
				s_Data.EffectOrder.erase(orderIt);
			}
		}
	}

	void PostProcessingSystem::EnableEffect(const std::string& effectName, bool enabled)
	{
		auto it = s_Data.Effects.find(effectName);
		if (it != s_Data.Effects.end())
		{
			it->second->SetEnabled(enabled);
		}
	}

	Ref<PostProcessingEffect> PostProcessingSystem::GetEffect(const std::string& effectName)
	{
		auto it = s_Data.Effects.find(effectName);
		if (it != s_Data.Effects.end())
		{
			return it->second;
		}

		return nullptr;
	}

	void PostProcessingSystem::ApplyEffects(CommandList& cmd, Ref<Framebuffer> source, Ref<Framebuffer> destination)
	{
		if (s_Data.Effects.empty() || s_Data.EffectOrder.empty())
		{
			// No effects to apply, just copy source to destination
			// (Implement a direct copy or skip post-processing)
			return;
		}

		// Ensure intermediate framebuffer is the right size
		if (s_Data.IntermediateFramebuffer->GetWidth() != source->GetWidth() ||
			s_Data.IntermediateFramebuffer->GetHeight() != source->GetHeight())
		{
			s_Data.IntermediateFramebuffer->Resize(source->GetWidth(), source->GetHeight());
		}

		// Apply all enabled effects in sequence
		Ref<Framebuffer> currentSource = source;
		Ref<Framebuffer> currentDestination;

		bool isLastEffect = false;
		int enabledEffectCount = 0;

		// Count enabled effects
		for (const auto& effectName : s_Data.EffectOrder)
		{
			auto effect = s_Data.Effects[effectName];
			if (effect->IsEnabled())
			{
				enabledEffectCount++;
			}
		}

		int currentEffectIndex = 0;

		for (const auto& effectName : s_Data.EffectOrder)
		{
			auto effect = s_Data.Effects[effectName];
			if (!effect->IsEnabled())
				continue;

			currentEffectIndex++;
			isLastEffect = (currentEffectIndex == enabledEffectCount);

			// If this is the last effect, output to final destination
			// Otherwise output to the intermediate buffer
			currentDestination = isLastEffect ? destination : s_Data.IntermediateFramebuffer;

			// Apply the effect
			effect->Apply(cmd, currentSource, currentDestination);

			// For the next iteration, if not the last effect
			if (!isLastEffect)
			{
				currentSource = s_Data.IntermediateFramebuffer;
			}
		}
	}
}