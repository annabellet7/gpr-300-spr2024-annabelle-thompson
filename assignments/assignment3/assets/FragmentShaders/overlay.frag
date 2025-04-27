/*References:
 *https://en.wikipedia.org/wiki/Blend_modes#Overlay
 */

#version 450

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uScreenTexture;
uniform sampler2D uPaper;

uniform float uMix = 0.5;

float brightness (vec3	color)
{
	return max(color.r, max(color.g, color.b));
}

void main()
{
	vec3 col = texture(uScreenTexture, TexCoord).rgb;

	//if the base layer is light the top layer becomes lighter
	//if the base layer is dark the top layer becomes darker
	if (brightness(col) < 0.5)
	{
		col = 2 * col * texture(uPaper, TexCoord).rgb;
	}
	else
	{
		col = 1.0 - 2.0 * (1.0 - col)*(1 - texture(uPaper, TexCoord).rgb);
	}

    FragColor = vec4(col, 1.0);
	
}