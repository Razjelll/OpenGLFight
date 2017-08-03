#version 330 core
layout(location = 0 ) out vec4 vFragColor;
uniform vec3 vColor;
void main()
{
    vFragColor = vec4(vColor.xyz,1);
}