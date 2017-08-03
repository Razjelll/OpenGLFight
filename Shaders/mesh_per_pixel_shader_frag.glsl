#version 330

struct MaterialParameters
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

MaterialParameters GetMaterial(const int material);
vec4 BlinnPhongLight(const int light, const vec3 normal, const vec3 position, const vec4 ambient, const vec4 diffuse, const vec4 specular, const float shininess);

uniform int material;
uniform sampler2D textureMap;
in vec3 inoutNormal;
in vec3 inoutPosition;
out vec4 outColor;
in vec2 inoutUV;

void main()
{
	vec3 normal = gl_FrontFacing ? normalize(inoutNormal) : -normalize(inoutNormal);
	MaterialParameters mat = GetMaterial(material);
	vec4 tempColor= BlinnPhongLight(0, normal, inoutPosition, mat.ambient, mat.diffuse, mat.specular, mat.shininess);
	//outColor = vec4(1,1,1,1);
	outColor =  tempColor * texture(textureMap, inoutUV);
}
