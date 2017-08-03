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

//light - numer Ÿród³a œwiat³a
//normal - znormalizowany wektor normalny
//position - wspó³rzêdne wierzcho³ka
// diffuse - wspó³czynnik odbicia œwiat³a rozproszonego
vec4 LambertDirectionalLight(const int light, const vec3 normal, const vec3 position, const vec4 diffuse)
{
	LightSourceParameters lightSource = LightSource(light);
	//obliczanie cosinusa k¹ta odbicia œwiat³a
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

//ambient - wpsó³czynnik odbicia œwiat³a otoczenia 
vec4 LambertPointLight(const int light, const vec3 normal, const vec3 position, const vec4 ambient, const vec4 diffuse)
{
	LightSourceParameters lightSource - LightSource(light);
	//wektor z bie¿¹cego wierzcho³ka do Ÿród³a œwiat³a
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

// model oœwietlenia dla pojedyñczego Ÿród³a œwiat³a wraz z globalnym i lokalnym œwiat³em otoczenia
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