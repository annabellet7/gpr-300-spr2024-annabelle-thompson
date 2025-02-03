#version 450

out vec4 FragColor;

in vec2 TexCoord;

uniform  sampler2D uScreenTexture;

void main()
{
	FragColor = texture(uScreenTexture, TexCoord);
}