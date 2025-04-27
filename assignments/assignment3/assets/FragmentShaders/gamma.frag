#version 450

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uScreenTexture;

uniform float uGamma;
uniform bool uGammaOn;

void main()
{

	vec3 col = texture(uScreenTexture, TexCoord).rgb;

	if (uGammaOn)
		col = pow(col, vec3(1.0/uGamma));

    FragColor = vec4(col, 1.0);
	
}