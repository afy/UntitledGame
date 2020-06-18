#version 330 core
layout(location = 0) in vec3 vpos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	gl_Position = model * projection * view * vec4(vpos, 1.0f);
}