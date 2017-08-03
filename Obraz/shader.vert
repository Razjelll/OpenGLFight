#version 330 core
layout(location = 0) in vec2 vVertex;
smooth out vec2 vUV;
uniform mat4 MVP;
void main()
{
	gl_Position = vec4(vVertex,0,1);
	vUV = vVertex;
}