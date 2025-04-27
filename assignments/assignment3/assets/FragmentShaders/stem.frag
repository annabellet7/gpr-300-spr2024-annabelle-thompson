#version 450

out vec4 FragColor;

in Surface 
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
}fs_in;

uniform sampler2D uMainTex;

uniform vec3 uLightColor;
uniform vec3 uAmbientColor = vec3(0.3, 0.4, 0.46);

uniform vec3 uEyePos;
uniform vec3 uLightDir;

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
    vec3 ambient = uMaterial.Ka * uLightColor;
    // diffuse
    vec3 lightDir = normalize(uLightDir);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * uLightColor;
    // specular

    vec3 viewDir = normalize(uEyePos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm,halfwayDir),0.0),uMaterial.Shininess); 
    vec3 specular = spec * uLightColor;    


    vec3 lightColor = (diffuse * uMaterial.Kd + specular * uMaterial.Ks) * uLightColor;
	lightColor += uAmbientColor * uMaterial.Ka; 
    
    FragColor = vec4(color * lightColor, 1.0);
    //FragColor = vec4(fs_in.TexCoord.x, fs_in.TexCoord.y, 0.0, 0.0);
}