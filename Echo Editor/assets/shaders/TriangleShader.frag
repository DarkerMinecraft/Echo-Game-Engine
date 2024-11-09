#version 450 

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

//layout(push_constant) uniform Push {
//	mat2 Transform;
//	vec2 Offset;
//	vec3 Color;
//} push;

void main() 
{
	outColor = vec4(fragColor, 1.0);
}