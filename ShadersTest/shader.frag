#version 330 core
  
layout(location=0) out vec4 vFragColor;

uniform mat4 MV;			   
uniform sampler2D textureMap;  
uniform float useDefault;	   
uniform vec3 light_position;   
 
smooth in vec3 vEyeSpaceNormal;   
smooth in vec3 vEyeSpacePosition; 
smooth in vec2 vUVout;			  

const float k0 = 1.0;	
const float k1 = 0.0;	
const float k2 = 0.0;	

void main()
{ 

	vec4 vEyeSpaceLightPosition = (MV*vec4(light_position,1));

	vec3 L = (vEyeSpaceLightPosition.xyz-vEyeSpacePosition);

	float d = length(L);

	L = normalize(L);

	float diffuse = max(0, dot(vEyeSpaceNormal, L));	

	float attenuationAmount = 1.0/(k0 + (k1*d) + (k2*d*d));
	diffuse *= attenuationAmount;

	vFragColor = diffuse*mix(texture(textureMap, vUVout), vec4(1), useDefault);
	//vFragColor = vec4(1,1,1,1);
}