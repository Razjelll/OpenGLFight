#version 330
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
#define POSITION 0
#define NORMAL 1
#define UV 2
layout(location = POSITION) in vec3 inPosition;
layout(location = NORMAL) in vec3 inNormal;
layout(location = UV) in vec2 inUV;
out vec2 inoutUV;
out vec3 inoutNormal;
out vec3 inoutPosition;

const vec4 positions[4] = vec4[4]
(
    vec4( -3.0, -3.0,  0.0,  0.0 ),
    vec4( -1.0, -1.0, -1.33, 0.0 ),
    vec4( 1.0,  1.0, -2.66, 0.0 ),
    vec4( 3.0,  3.0, -4.0,  0.0 )
);

out Vertex
{
	vec4 position;
	vec3 normal;
	int material;
	vec2 uv;
} Out;

void main()
{
	inoutUV = inUV;
    gl_Position = modelViewProjectionMatrix * vec4(inPosition,1);
	//wyœwietlanie iwelu instancji na raz, nie chce dzia³aæ
	//gl_Position = modelViewProjectionMatrix *( vec4(inPosition,1) + positions[gl_InstanceID]);
    inoutNormal = normalize(normalMatrix * inNormal);
    inoutPosition = vec3(modelViewMatrix * vec4(inPosition,1));
}