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

const int maxLights = 4;

uniform LightSourceParameters lightSource[maxLights] =  LightSourceParameters[maxLights]
(
    LightSourceParameters( vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 1.0, 1.0, 1.0, 1.0 ),
                           vec4( 1.0, 1.0, 1.0, 1.0 ),
                           vec4( 0.0, 0.0, 1.0, 0.0 ),
                           1.0,
                           0.0,
                           0.0 ),
    LightSourceParameters( vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 1.0, 0.0 ),
                           1.0,
                           0.0,
                           0.0 ),
    LightSourceParameters( vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 1.0, 0.0 ),
                           1.0,
                           0.0,
                           0.0 ),
    LightSourceParameters( vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 0.0, 1.0 ),
                           vec4( 0.0, 0.0, 1.0, 0.0 ),
                           1.0,
                           0.0,
                           0.0 )
);

LightSourceParameters LightSource( const int light )
{
    return lightSource[light];
}

struct LightModelParameters
{
    vec4 ambient;   
};

uniform LightModelParameters lightModel = LightModelParameters
(
    vec4( 0.2, 0.2, 0.2, 1.0 )
);

uniform vec4 eyePosition = vec4( 0.0, 0.0, 0.0, 1.0 );

vec4 EyePosition()
{
    return eyePosition;
}

vec4 GlobalAmbientLight( const vec4 ambient )
{
    return lightModel.ambient * ambient;
}

vec4 LocalAmbientLight( const int light, const vec4 ambient )
{
    return lightSource[light].ambient * ambient;
}
