#version 330 core

layout (location = 0) out vec4 vFragColor; 
uniform vec4 color;

void main()
{
	vFragColor = color;
}