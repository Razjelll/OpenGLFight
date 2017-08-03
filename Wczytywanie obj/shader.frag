#version 330 core
 out vec4 outColor;
in vec4 inoutColor;
void main()
{
	//vec4 temp = inoutColor;
    outColor = inoutColor;
	//outColor = vec4(1,1,1,1);
}