#version 450

layout (location = 0) in vec3 vPos; 
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTangent;
layout (location = 3) in vec2 vTexCoord;

uniform mat4 uModel;
uniform mat4 uViewProjection;
uniform mat4 uLightSpaceMatrix;

uniform float uOutlineWidth;

out Surface 
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
}vs_out;

void main()
{
	vec3 pos = vPos + vNormal * uOutlineWidth;
	vs_out.FragPos = vec3(uModel * vec4(pos, 1.0));
	vs_out.TexCoord = vTexCoord;
	vs_out.Normal = vNormal;

	gl_Position = uViewProjection * uModel * vec4(pos, 1.0);
}	