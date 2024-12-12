#type vertex
#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec4 color;

layout(location = 0) out vec2 outTexCoords;

layout(push_constant) uniform constants
{
	mat4 transformationMatrix;
	mat4 projViewMatrix;
};

void main()
{
	gl_Position = projViewMatrix * transformationMatrix * vec4(position, 1.0f);

	outTexCoords = texCoords;
}

#type pixel
#version 450

layout(location = 0) in vec2 outTexCoords;

layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform sampler2D displayTexture;

void main()
{
	fragColor = texture(displayTexture, outTexCoords);
}