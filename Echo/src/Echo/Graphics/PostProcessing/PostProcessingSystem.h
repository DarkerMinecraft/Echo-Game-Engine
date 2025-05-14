#pragma once

#include "Graphics/Framebuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Pipeline.h"
#include "Graphics/CommandList.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace Echo
{
	// Forward declaration
	class PostProcessingEffect;

	class PostProcessingSystem
	{
	public:
		static void Init();
		static void Shutdown();

		// Add a post-processing effect
		static void AddEffect(const Ref<PostProcessingEffect>& effect);

		// Remove a post-processing effect
		static void RemoveEffect(const std::string& effectName);

		// Enable/disable an effect
		static void EnableEffect(const std::string& effectName, bool enabled);

		// Get a specific effect
		static Ref<PostProcessingEffect> GetEffect(const std::string& effectName);

		// Apply all enabled effects
		static void ApplyEffects(CommandList& cmd, Ref<Framebuffer> source, Ref<Framebuffer> destination);

		// Get the fullscreen quad buffer
		static Ref<VertexBuffer> GetFullscreenQuadVertexBuffer() { return s_Data.FullscreenQuadVertexBuffer; }
		static Ref<IndexBuffer> GetFullscreenQuadIndexBuffer() { return s_Data.FullscreenQuadIndexBuffer; }

	private:
		struct PostProcessingSystemData
		{
			std::unordered_map<std::string, Ref<PostProcessingEffect>> Effects;
			std::vector<std::string> EffectOrder; // Order in which effects are applied

			Ref<VertexBuffer> FullscreenQuadVertexBuffer;
			Ref<IndexBuffer> FullscreenQuadIndexBuffer;

			Ref<Framebuffer> IntermediateFramebuffer; // Used for chaining effects
		};

		static PostProcessingSystemData s_Data;
	};

	// Base class for all post-processing effects
	class PostProcessingEffect : public std::enable_shared_from_this<PostProcessingEffect>
	{
	public:
		PostProcessingEffect(const std::string& name) : m_Name(name), m_Enabled(true) {}
		virtual ~PostProcessingEffect() = default;

		// Apply this effect
		virtual void Apply(CommandList& cmd, Ref<Framebuffer> source, Ref<Framebuffer> destination) = 0;

		// Initialize effect resources
		virtual void Init() = 0;

		// Clean up effect resources
		virtual void Cleanup() = 0;

		// Enable/disable this effect
		void SetEnabled(bool enabled) { m_Enabled = enabled; }
		bool IsEnabled() const { return m_Enabled; }

		// Get the effect name
		const std::string& GetName() const { return m_Name; }

	protected:
		std::string m_Name;
		bool m_Enabled;

		Ref<Shader> m_Shader;
		Pipeline* m_Pipeline;
		Ref<UniformBuffer> m_UniformBuffer;
	};
}