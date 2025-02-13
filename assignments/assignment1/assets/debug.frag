#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uDepthMap;
uniform float uNearPlane;
uniform float uFarPlane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;  
    return (2.0 * uNearPlane * uFarPlane) / (uFarPlane + uNearPlane - z * (uFarPlane - uNearPlane));	
}

void main()
{             
    float depthValue = texture(uDepthMap, TexCoord).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}