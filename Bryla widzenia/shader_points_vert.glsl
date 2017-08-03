#version 330 core
layout(location = 0) in vec3 vVertex;
uniform float t;
const float PI = 3.141562;
void main()
{
	gl_Position = vec4(vVertex,1)+vec4(0,sin(vVertex.x*2*PI+t),0,0);
}