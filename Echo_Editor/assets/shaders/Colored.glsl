#type vertex
#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec4 color;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoords;

layout(push_constant) uniform constants
{
	mat4 transformationMatrix;
	mat4 projViewMatrix;
};

void main() 
{
	gl_Position = projViewMatrix * transformationMatrix * vec4(position, 1.0f);

	outColor = color.xyz;
	outTexCoords = texCoords;
}

#type pixel
#version 450 

layout(location = 0) in vec3 outColor;
layout(location = 1) in vec2 outTexCoords;

layout(location = 0) out vec4 fragColor;

void main() 
{
	fragColor = vec4(outColor, 1.0);
}