#version 450

layout (location = 0) in vec3 vPos; 
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTangent;
layout (location = 3) in vec2 vTexCoord;

uniform mat4 uModel;
uniform mat4 uViewProjection;

uniform vec3 uEyePos;
uniform vec3 uLightDir;

out Surface 
{
	vec3 TangentFragPos;
	vec2 TexCoord;
	vec3 TangentEyePos;
	vec3 TangnetLightDir;
}vs_out;

void main()
{
	mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    vec3 tangent = normalize(normalMatrix * vTangent);
    vec3 normal = normalMatrix * vNormal;
    vec3 bitangent = normalize(cross(normal, tangent));

	mat3 TBN = transpose(mat3(tangent, bitangent, normal));
	vs_out.TangentEyePos = TBN * uEyePos;
	vs_out.TangnetLightDir = TBN * normalize(uLightDir);
	vs_out.TangentFragPos = TBN * vec3(uModel * vec4(vPos, 1.0));

	vs_out.TexCoord = vTexCoord;

	gl_Position = uViewProjection * uModel * vec4(vPos, 1.0);
}