#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out float real;
layout(location = 1) out float img;
void main() {
	gl_Position = vec4(inPosition, 1.0);
	real = inPosition.x * 2.5;
	img  = inPosition.y * 2.5;}