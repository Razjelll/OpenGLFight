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

LightSourceParameters LightSource(const int light);
vec4 EyePosition();
vec4 GlobalAmbientLight(const vec4 ambient);
vec4 LocalAmbientLight(const int light, const vec4 ambient);

vec4 BlinnPhongDirectionalLight(const int light, const vec3 normal, const vec3 position, const vec4 diffuse, const vec4 specular, const float shininess)
{
	LightSourceParameters lightSource = LightSource(light);
	float NdotL = dot(normal, lightSource.position.xyz);
	if(NdotL > 0.0)
	{
		vec3 halfVec = normalize(lightSource.position.xyz + normalize(EyePosition().xyz - position));
		float NdotH = max(dot(normal, halfVec), 0.0);
		return NdotL * diffuse * lightSource.diffuse + specular * lightSource.specular * pow(NdotH, shininess);
	} 
	else
	{
		return vec4(0.0);
	}
	
}

vec4 BlinnPhongPointLight(const int light, const vec3 normal, const vec3 position, const vec4 ambient, const vec4 diffuse, const vec4 specular, const float shininess)
{
	LightSourceParameters lightSource = LightSource(light);
	vec3 lightVec = lightSource.position.xyz - position;
	float distance = length(lightVec);
	lightVec = normalize(lightVec);
	float NdotL = dot(normal, lightVec);
	if(NdotL > 0.0)
	{
		float att = 1.0 / ( lightSource.constantAttenuation +
                            lightSource.linearAttenuation * distance +
                            lightSource.quadraticAttenuation * distance * distance );
		vec3 halfVec = normalize(lightVec + normalize (EyePosition().xyz - position));
		float NdotH = max(dot(normal, halfVec), 0.0);
		return att * (LocalAmbientLight(light, ambient) + NdotL * diffuse * lightSource.diffuse + specular * lightSource.specular * pow(NdotH, shininess));
	}
	else
	{
		return vec4(0.0);
	}
}

vec4 BlinnPhongLight( const int light, const vec3 normal, const vec3 position,
                        const vec4 ambient, const vec4 diffuse,
                        const vec4 specular, const float shininess )
{
    if( LightSource( light ).position.w == 0.0 )
        return BlinnPhongDirectionalLight( light, normal, position, diffuse, specular, shininess ) +
                LocalAmbientLight( light, ambient ) + GlobalAmbientLight( ambient );
    else
        return BlinnPhongPointLight( light, normal, position, ambient, diffuse, specular, shininess ) +
                GlobalAmbientLight( ambient );
}