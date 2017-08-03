#version 150

struct LightSourceParameters
{
	vec4 ambient; //intensywno�� �wiat�a otoczenia, La
	vec4 diffuse; //intensywno�� �wiat�a rozproszonego, Ld
	vec4 specular; //intensywno�� �wiat�a zwierciadlanego; Ls
	vec4 position; //po�o�enie �r�d�� �wiat�a punktowego (w = 1)lub wektor kierunku �wiat�a kierunkowego (w=0)
	float constantAttenuation; //sta�y wsp�czynnik t�umienia �wiat�a; kc
	float linearAttenuation; //liniowy wsp�czynnik t�umienia �wiat�a; kl
	float quadraticAttenuation; //kwadratowy wsp�czynnik t�umienia �wiat�a, kq
};

const int maxLights = 4;

struct LightModelParameters
{
	vec4 ambient; //globalne �wiat�o otoczenia, Ga
};

layout(shared) uniform LightModel
{
	LightSourceParameters lightSource[maxLights];
	LightModelParameters lightModel;
	//wsp�cz�dne po�o�enia obserwatora
	vec4 eyePosition;
};

LightSourceParameters LightSource(const int light)
{
	return lightSource[light];
}

vec4 EyePosition()
{
	return eyePosition;
}

vec4 GlobalAmbientLight (const vec4 ambient)
{
	return lightModel.ambient * ambient;
}

vec4 LocalAmbientLight(const int light, const vec4 ambient)
{
	return lightSource[light].ambient*ambient;
}