#version 450

out vec4 FragColor;

in Surface 
{
	vec3 Normal;
	vec2 TexCoord;
}fs_in;

uniform sampler2D uMainTex;

void main()
{
	FragColor = texture(uMainTex, fs_in.TexCoord);
}