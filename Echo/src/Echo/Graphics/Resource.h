#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

namespace Echo 
{
	enum class ImageFormat
	{
		Undefined = 0,
		R8_UNORM = 9,
		R8_SNORM = 10,
		R8_UINT = 13,
		R8_SINT = 14,
		R8G8_UNORM = 16,
		R8G8_SNORM = 17,
		R8G8_UINT = 20,
		R8G8_SINT = 21,
		R8G8B8A8_UNORM = 37,
		R8G8B8A8_SNORM = 38,
		R8G8B8A8_UINT = 41,
		R8G8B8A8_SINT = 42,
		R8G8B8A8_SRGB = 43,
		B8G8R8A8_UNORM = 44,
		B8G8R8A8_SNORM = 45,
		B8G8R8A8_UINT = 48,
		B8G8R8A8_SINT = 49,
		B8G8R8A8_SRGB = 50,
		A2R10G10B10_UNORM_PACK32 = 58,
		R16_UNORM = 70,
		R16_SNORM = 71,
		R16_UINT = 74,
		R16_SINT = 75,
		R16_SFLOAT = 76,
		R16G16_UNORM = 77,
		R16G16_SNORM = 78,
		R16G16_UINT = 81,
		R16G16_SINT = 82,
		R16G16_SFLOAT = 83,
		R16G16B16A16_UNORM = 91,
		R16G16B16A16_SNORM = 92,
		R16G16B16A16_UINT = 95,
		R16G16B16A16_SINT = 96,
		R16G16B16A16_SFLOAT = 97,
		R32_UINT = 98,
		R32_SINT = 99,
		R32_SFLOAT = 100,
		R32G32_UINT = 101,
		R32G32_SINT = 102,
		R32G32_SFLOAT = 103,
		R32G32B32A32_UINT = 107,
		R32G32B32A32_SINT = 108,
		R32G32B32A32_SFLOAT = 109,
		B10G11R11_UFLOAT_PACK32 = 122,
		E5B9G9R9_UFLOAT_PACK32 = 123,
		D16_UNORM = 124,
		D32_SFLOAT = 126,
		D24_UNORM_S8_UINT = 129,
		D32_SFLOAT_S8_UINT = 130,
		BC1_RGB_UNORM_BLOCK = 131,
		BC1_RGB_SRGB_BLOCK = 132,
		BC1_RGBA_UNORM_BLOCK = 133,
		BC1_RGBA_SRGB_BLOCK = 134,
		BC2_UNORM_BLOCK = 135,
		BC2_SRGB_BLOCK = 136,
		BC3_UNORM_BLOCK = 137,
		BC3_SRGB_BLOCK = 138,
		BC4_UNORM_BLOCK = 139,
		BC4_SNORM_BLOCK = 140,
		BC5_UNORM_BLOCK = 141,
		BC5_SNORM_BLOCK = 142,
		BC6H_UFLOAT_BLOCK = 143,
		BC6H_SFLOAT_BLOCK = 144,
		BC7_UNORM_BLOCK = 145,
		BC7_SRGB_BLOCK = 146,
	};

	enum class AssetResource 
	{
		GraphicsShader, 
		RaytracingShader, 
		ComputeShader,
		Texture
	};

	struct ResourceCreateInfo 
	{
		AssetResource Resource;
		std::string VertexShader; 
		std::string FragmentShader;
	};

	struct Vertex 
	{
		glm::vec3 Position;
		glm::vec3 Color;
	};

	class Resource 
	{
	public:
		Resource() = default;
		virtual ~Resource() = default;

		virtual void Bind() = 0;
		virtual void LoadVertex(Vertex vertex) = 0;
		virtual void Unbind() = 0;

		static Ref<Resource> Create(const ResourceCreateInfo& createInfo); 
	};
}