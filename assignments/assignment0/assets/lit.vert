#version 450

layout (location = 0) in vec3 vPos; 
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 uModel;
uniform mat4 uViewProjection;

out Surface 
{
	vec3 Normal;
	vec2 TexCoord;
}vs_out;

void main()
{
	vs_out.Normal = vNormal;
	vs_out.TexCoord = vTexCoord;
	gl_Position = uViewProjection * uModel * vec4(vPos, 1.0);
}