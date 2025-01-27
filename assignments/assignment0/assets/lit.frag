#version 450

out vec4 FragColor;

in Surface 
{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
}fs_in;

uniform sampler2D uMainTex;

uniform vec3 uEyePos;
uniform vec3 uLightDir = vec3(0.0, -1.0, 0.0);
uniform vec3 uLightColor = vec3(1.0);

void main()
{
	vec3 norm = normalize(fs_in.WorldNormal);
	vec3 toLight = -uLightDir;
	float diffuseFactor = max(dot(norm, toLight), 0.0);

	vec3 toEye = normalize(uEyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(norm,h),0.0),128); 

	vec3 lightColor = uLightColor * (diffuseFactor + specularFactor);
	vec3 objColor = texture(uMainTex, fs_in.TexCoord).rgb;

	FragColor = vec4(lightColor * objColor, 1.0);
}