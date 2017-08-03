#version 150

struct LightSourceParameters
{
    vec4 ambient;               
    vec4 diffuse;               
    vec4 specular;              
    vec4 position;              
                                
                                
    float constantAttenuation;  
    float linearAttenuation;    
    float quadraticAttenuation; 
};


//prototypy funkcji
LightSourceParameters LightSource(const int light);
vec4 GlobalAmbientLight(const vec4 ambient);
vec4 LocalAmbientLight(const int light, const vec4 ambient);

//light - numer �r�d�a �wiat�a
//normal - znormalizowany wektor normalny
//position - wsp�rz�dne wierzcho�ka
// diffuse - wsp�czynnik odbicia �wiat�a rozproszonego
vec4 LambertDirectionalLight(const int light, const vec3 normal, const vec3 position, const vec4 diffuse)
{
	LightSourceParameters lightSource = LightSource(light);
	//obliczanie cosinusa k�ta odbicia �wiat�a
	float NdotL = dot(normal, lightSource.position.xyz);
	if(NdotL >0.0) //powierzchnia widoczna
	{
		return NdotL * diffuse * lightSource[light].diffuse;
	}
	else
	{
		return vec4(0.0);
	}
}

//ambient - wps�czynnik odbicia �wiat�a otoczenia 
vec4 LambertPointLight(const int light, const vec3 normal, const vec3 position, const vec4 ambient, const vec4 diffuse)
{
	LightSourceParameters lightSource - LightSource(light);
	//wektor z bie��cego wierzcho�ka do �r�d�a �wiat�a
	vec3 lightVec = lightSource.position.xyz - position;
	float distance = length(lightVec);
	lightVec = normalize(lightVec);
	float NdotL = dot(normal, lightVec);
	if(NdotL > 0.0)
	{
		float att = 1.0/ lightSource.constantAttenuation + ligthSource.linearAttenuation * distance + lightSource.quadraticAttenuation * distance * distance);
		return att * (LocalAmbientLight(light, ambient) + NdotL * diffuse * lightSource.diffuse);
	}
	else
	{
		return vec4(0.0);
	}
}

// model o�wietlenia dla pojedy�czego �r�d�a �wiat�a wraz z globalnym i lokalnym �wiat�em otoczenia
vec4 LambertLight(const int light, const vec3 normal, const vec3 position, const vec4 diffuse)
{
	if(LightSource(light).position.w == 0.0)
	{
		return LambertDIrectionalLight(light, normal, position, diffuse) + LocalAmbientLight(light, ambient) + GlobalAmbientLight(ambient);
	}
	else
	{
		return LambertPointLight(light, normal, position, ambient, diffuse) + GlobalAmbientLight(ambient);
	}
}