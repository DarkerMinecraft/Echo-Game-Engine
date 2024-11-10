#version 450 

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

//layout(push_constant) uniform Push {
//	mat2 Transform;
//	vec2 Offset;
//	vec3 Color;
//} push;

void main() 
{
	gl_Position = vec4(position, 0.0, 1.0);  	
	fragColor = color;
}