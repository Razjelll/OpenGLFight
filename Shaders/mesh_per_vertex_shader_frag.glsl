#version 330

in vec4 inoutFrontColor;
int vec4 inoutBackColor;

out vec4 outColor;

void main()
{
	outColor = gl_frontFacing ? inoutFrontColor : inoutBackColor;
}