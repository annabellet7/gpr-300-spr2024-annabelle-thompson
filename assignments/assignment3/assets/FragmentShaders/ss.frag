#version 450

out vec4 FragColor;

in Surface 
{
	vec3 TangentFragPos;
	vec2 TexCoord;
	vec3 TangentEyePos;
	vec3 TangnetLightDir;
    vec3 Normal;
}fs_in;

uniform sampler2D uMainTex;

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
    vec3 color = texture(uMainTex, fs_in.TexCoord).rgb;
    vec3 norm = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.15 * uLightColor;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangnetLightDir - fs_in.TangentFragPos);
    float diff = max(dot(fs_in.TangnetLightDir, norm), 0.0);
    vec3 diffuse = diff * uLightColor;
    // specular
    vec3 viewDir = normalize(fs_in.TangentEyePos - fs_in.TangentFragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm,halfwayDir),0.0),uMaterial.Shininess); 
    vec3 specular = spec * uLightColor;    
    // calculate shadow

    vec3 lightColor = (diffuse * uMaterial.Kd + specular * uMaterial.Ks) * uLightColor;
	lightColor += uAmbientColor * uMaterial.Ka; 
    
    FragColor = vec4(color * lightColor, 1.0);  
}