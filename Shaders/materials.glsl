#version 150

struct MaterialParameters
{
	vec4 ambient; //wspó³czynnik odbicia œwiat³a otoczenia, Ma
	vec4 diffuse; //wspó³czynnik odbicia œwiat³a rozproszonego, Md
	vec4 specular; //wspó³czynnik odbicia œwiat³a zwierciedlanego, Ms
	float shininess; //wyk³adnik wspó³czynnika funkcji rozb³ysku, <0;128> Msh
};

#define MTL_DEFAULT              0  
#define MTL_BRASS                1  
#define MTL_BRONZE               2  
#define MTL_POLISHED_BRONZE      3  
#define MTL_CHROME               4  
#define MTL_COPPER               5  
#define MTL_POLISHED_COPPER      6  
#define MTL_GOLD                 7  
#define MTL_POLISHED_GOLD        8  
#define MTL_PEWTER               9  
#define MTL_SILVER              10  
#define MTL_POLISHED_SILVER     11  
#define MTL_EMERALD             12  
#define MTL_JADE                13  
#define MTL_OBSIDIAN            14  
#define MTL_PEARL               15  
#define MTL_RUBY                16  
#define MTL_TURQUOISE           17  
#define MTL_BRIGHT_WHITE        18  
#define MTL_LESS_BRIGHT_WHITE   19  
#define MTL_WARMISH_WHITE       20  
#define MTL_COOLISH_WHITE       21  
#define MTL_BLACK_PLASTIC       22  
#define MTL_CYAN_PLASTIC        23  
#define MTL_GREEN_PLASTIC       24  
#define MTL_RED_PLASTIC         25  
#define MTL_WHITE_PLASTIC       26  
#define MTL_YELLOW_PLASTIC      27  
#define MTL_BLACK_RUBBER        28  
#define MTL_CYAN_RUBBER         29  
#define MTL_GREEN_RUBBER        30  
#define MTL_RED_RUBBER          31  
#define MTL_WHITE_RUBBER        32  
#define MTL_YELLOW_RUBBER       33  

layout (shared) uniform Materials
{
	MaterialParameters materials[34];
};

MaterialParameters GetMaterial(const int material)
{
	if(material >= MTL_DEFAULT && material <= MTL_YELLOW_RUBBER)
	{
		return materials[material];
	}
	else 
	{
		return materials[MTL_DEFAULT];
	}
}