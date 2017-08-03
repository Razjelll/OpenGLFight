#version 330 core
layout(location = 0) out vec4 vFragColor;
smooth in vec2 vUV;
uniform sampler2D textureMap;
//const float kernel[] = float[9] (-1,-1,-1,-1,8,-1,-1,-1,-1);
const float kernel[] = float[9] (0,0,0,-4,12,0,-4,-4,0);


void main()
{
	vec2 delta = 1.0/textureSize(textureMap, 0);
	vec4 color = vec4(0);
	int index = 8;

	for(int i =-1; i<1; i++)
	{
		for(int j=-1; j<1; j++)
		{
			color += kernel[index--]*texture(textureMap, vUV+(vec2(j,1) * delta));
		}
	
	}

	color /=9.0;
	vFragColor = color+texture(textureMap, vUV);
}