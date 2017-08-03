#version 330 core
layout(triangles)in;
layout(triangle_strip, max_vertices=256) out;
uniform int sub_divisions;
uniform mat4 MVP;
uniform float time;

const float amplitude = 0.3;
const float frequency = 5;
const float PI = 3.14159;
void main()
{
	vec4 v0 = gl_in[0].gl_Position;
	vec4 v1 = gl_in[1].gl_Position;
	vec4 v2 = gl_in[2].gl_Position;
	float dx = abs(v0.x-v2.x)/sub_divisions;
	float dz = abs(v0.z-v1.z)/sub_divisions;
	float x=v0.x;
	float z=v0.z;


	for(int j=0; j<sub_divisions*sub_divisions;j++)
	{

		float distance = length(vec3(x,0,z));
		float y = amplitude*sin(-PI*distance*frequency+time);
		

		gl_Position = MVP * vec4(x,y,z,1);
		EmitVertex();
		 distance = length(vec3(x,0,z+dz));
		 y = amplitude*sin(-PI*distance*frequency+time);

		gl_Position = MVP * vec4(x,y,z+dz,1);
		EmitVertex();

		 distance = length(vec3(x+dx,0,z));
		 y = amplitude*sin(-PI*distance*frequency+time);

		gl_Position = MVP * vec4(x+dx, y, z,1);
		EmitVertex();

		 distance = length(vec3(x+dx,0,z+dz));
		 y = amplitude*sin(-PI*distance*frequency+time);

		gl_Position = MVP * vec4(x+dx, y, z+dz,1);
		EmitVertex();
		EndPrimitive();
		x+=dx;
		if((j+1)%sub_divisions == 0)
		{
			x=v0.x;
			z+=dz;
		}
		/*gl_Position = MVP * gl_in[0].gl_Position; EmitVertex();
		gl_Position = MVP * gl_in[1].gl_Position; EmitVertex();
		gl_Position = MVP * (gl_in[0].gl_Position + gl_in[2].gl_Position); EmitVertex();
		gl_Position = MVP * gl_in[2].gl_Position; EmitVertex();
		EndPrimitive();*/

		

	}
	/*gl_Position = MVP * gl_in[0].gl_Position; EmitVertex();
	gl_Position = MVP * gl_in[1].gl_Position; EmitVertex();
	gl_Position = MVP * gl_in[2].gl_Position; EmitVertex();
	EndPrimitive();*/
}