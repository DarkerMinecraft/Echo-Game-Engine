#pragma once
#include "PostProcessingSystem.h"

namespace Echo
{
	class OutlineEffect : public PostProcessingEffect
	{
	public:
		OutlineEffect() : PostProcessingEffect("Outline") {}

		virtual void Init() override;
		virtual void Cleanup() override;
		virtual void Apply(CommandList& cmd, Ref<Framebuffer> source, Ref<Framebuffer> destination) override;

		// Setters for outline properties
		void SetSelectedEntityID(int entityID) { m_SelectedEntityID = entityID; }
		void SetOutlineColor(const glm::vec4& color) { m_OutlineColor = color; }
		void SetOutlineThickness(float thickness) { m_OutlineThickness = thickness; }

		// Getters for outline properties
		int GetSelectedEntityID() const { return m_SelectedEntityID; }
		const glm::vec4& GetOutlineColor() const { return m_OutlineColor; }
		float GetOutlineThickness() const { return m_OutlineThickness; }

	private:
		int m_SelectedEntityID = -1;
		glm::vec4 m_OutlineColor = { 1.0f, 0.35f, 0.0f, 1.0f }; // Orange outline
		float m_OutlineThickness = 1.0f; // Thickness in pixels
	};
}