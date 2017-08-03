#version 150

struct LightSourceParameters
{
	vec4 ambient; //intensywnoœæ œwiat³a otoczenia, La
	vec4 diffuse; //intensywnoœæ œwiat³a rozproszonego, Ld
	vec4 specular; //intensywnoœæ œwiat³a zwierciadlanego; Ls
	vec4 position; //po³o¿enie Ÿród³¹ œwiat³a punktowego (w = 1)lub wektor kierunku œwiat³a kierunkowego (w=0)
	float constantAttenuation; //sta³y wspó³czynnik t³umienia œwiat³a; kc
	float linearAttenuation; //liniowy wspó³czynnik t³umienia œwiat³a; kl
	float quadraticAttenuation; //kwadratowy wspó³czynnik t³umienia œwiat³a, kq
};

const int maxLights = 4;

struct LightModelParameters
{
	vec4 ambient; //globalne œwiat³o otoczenia, Ga
};

layout(shared) uniform LightModel
{
	LightSourceParameters lightSource[maxLights];
	LightModelParameters lightModel;
	//wspó³czêdne po³o¿enia obserwatora
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