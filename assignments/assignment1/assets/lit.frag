#version 450

out vec4 FragColor;

in Surface 
{
	vec3 TangentFragPos;
	vec2 TexCoord;
	vec3 TangentEyePos;
	vec3 TangnetLightDir;
}fs_in;

uniform sampler2D uMainTex;
uniform sampler2D uMainNorms;

uniform vec3 uLightColor;
uniform vec3 uAmbientColor = vec3(0.3, 0.4, 0.46);

struct Material
{
	float Ka; //ambient coefficient
	float Kd; //diffuse coefficient
	float Ks; //specular coefficient
	float Shininess;
};
uniform Material uMaterial;

void main()
{	
	vec3 norm = normalize(fs_in.TangentFragPos);
	vec3 normalMap = texture(uMainNorms, fs_in.TexCoord).rgb;
	normalMap = normalize(normalMap * 2.0 - 1.0);

	vec3 toLight = -fs_in.TangnetLightDir;
	float diffuseFactor = max(dot(normalMap, toLight), 0.0);

	vec3 toEye = normalize(fs_in.TangentEyePos - fs_in.TangentFragPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normalMap,h),0.0),uMaterial.Shininess); 

	vec3 lightColor = (diffuseFactor * uMaterial.Kd + specularFactor * uMaterial.Ks) * uLightColor;
	lightColor += uAmbientColor * uMaterial.Ka;
	vec3 objColor = texture(uMainTex, fs_in.TexCoord).rgb;

	FragColor = vec4(objColor * lightColor, 1.0);
}