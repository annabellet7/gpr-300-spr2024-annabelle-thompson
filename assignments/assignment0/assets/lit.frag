#version 450

out vec4 FragColor;

in Surface 
{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
}fs_in;

uniform sampler2D uMainTex;

uniform vec3 uLightDir = vec3(0.0, -1.0, 0.0);
uniform vec3 uLightColor = vec3(1.0);

void main()
{
	vec3 norm = normalize(fs_in.WorldNormal);
	vec3 toLight = -uLightDir;
	float diffuseFactor = max(dot(norm, toLight), 0.0);

	vec3 diffuseColor = uLightColor * diffuseFactor;
	vec3 objColor = texture(uMainTex, fs_in.TexCoord).rgb;

	FragColor = vec4(diffuseColor * objColor, 1.0);
}