#version 330

struct MaterialParameters
{
    vec4 ambient; //współczynnik odbicia światła otoczenia, Ma
    vec4 diffuse; //współczynnik odbicia światła rozproszonego, Md
    vec4 specular; //współczynnik światła zwierciadlanego, Ms
    float shininess;
}

MaterialParameters GetMaterial(const int material);
vec4 BlinnPhongLight(const int light, const vec3 normal, const vec3 position, const vec4 ambient, const vec4 diffuse, const vec4 specular, const float shininess);

uniform int material;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

#define POSITION 0
#define NORMAL 1

layout( location = POSITION) in vec4 inPosition;
layout(location = NORMAL) in vec3 inNormal;

out vec4 inoutFrontColor;
out vec4 inoutBackColor;

void main()
{
    gl_Position = modelViewProjectionMatrix * inPosition;
    vec3 normal = normalize(normalMatrix * inNormal);
    vec3 position = vec3(modelViewMatrix * inPosition);
    MaterialParameters mat = GetMaterial(material);
    inoutFrontColor = BlinnPhongLight(0, normal, position, mat.ambient, mat.diffuse, mat.specular, mat.shininess);
    inoutBackColor = BlinnPhongLight(0, -normal, position, mat.ambient, mat.diffuse, mat.specular, mat.shininess);

}
