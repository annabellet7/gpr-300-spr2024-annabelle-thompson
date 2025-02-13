#version 450

out vec4 FragColor;

in Surface 
{
	vec3 TangentFragPos;
	vec2 TexCoord;
	vec3 TangentEyePos;
	vec3 TangnetLightDir;
	vec4 FragPosLightSpace;
}fs_in;

uniform sampler2D uMainTex;
uniform sampler2D uMainNorms;
uniform sampler2D uShadowMap;

uniform vec3 uLightColor;
uniform vec3 uAmbientColor = vec3(0.3, 0.4, 0.46);
uniform float uGamma;
uniform float uTexel;

struct Material
{
	float Ka; //ambient coefficient
	float Kd; //diffuse coefficient
	float Ks; //specular coefficient
	float Shininess;
};
uniform Material uMaterial;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0)
    {
        return 0.0; 
    }

	float closestDepth = texture(uShadowMap, projCoords.xy).r;
	float currentDepth =  projCoords.z;

    vec3 normalMap = texture(uMainNorms, fs_in.TexCoord).rgb;
	normalMap = normalize(normalMap * 2.0 - 1.0);

    float bias = max(0.05 * (1.0 - dot(normalMap, fs_in.TangnetLightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = uTexel / textureSize(uShadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
	shadow /= 9.0;
	return shadow;
}

void main()
{	
    vec3 color = texture(uMainTex, fs_in.TexCoord).rgb;
    vec3 normalMap = texture(uMainNorms, fs_in.TexCoord).rgb;
	normalMap = normalize(normalMap * 2.0 - 1.0);
    // ambient
    vec3 ambient = 0.15 * uLightColor;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangnetLightDir - fs_in.TangentFragPos);
    float diff = max(dot(fs_in.TangnetLightDir, normalMap), 0.0);
    vec3 diffuse = diff * uLightColor;
    // specular
    vec3 viewDir = normalize(fs_in.TangentEyePos - fs_in.TangentFragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normalMap,halfwayDir),0.0),uMaterial.Shininess); 
    vec3 specular = spec * uLightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}