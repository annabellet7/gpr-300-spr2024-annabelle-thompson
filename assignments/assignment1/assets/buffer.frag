#version 450

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uScreenTexture;

uniform float uBlur;
uniform float uEdge;

uniform bool uBlurOn;
uniform bool uEdgeOn;

void main()
{
	vec3 col = vec3(texture(uScreenTexture, TexCoord));
	
	if (uBlurOn)
	{
		float offset = 1.0 / uBlur;

		vec2 offsets[9] = vec2[] 
			(
				vec2(-offset, offset),
				vec2(0.0, offset),
				vec2(offset, offset),
				vec2(-offset, 0.0),
				vec2(0.0, 0.0),
				vec2(offset, 0.0),
				vec2(-offset, -offset),
				vec2(0.0, -offset),
				vec2(offset, -offset)
			);

		float kernel[9] = float[]
		(
			1.0 / 16, 2.0 / 16, 1.0 / 16,
			2.0 / 16, 4.0 / 16, 2.0 / 16,
			1.0 / 16, 2.0 / 16, 1.0 / 16  
		);

		vec3 sampleTex[9];
		for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(uScreenTexture, TexCoord.st + offsets[i]));
		}

		col = vec3(0.0);
		for(int i = 0; i < 9; i++)
		{
			col += sampleTex[i] * kernel[i];
		}
	}

	if (uEdgeOn)
	{
		float offset = 1.0 / uEdge;

		vec2 offsets[9] = vec2[] 
			(
				vec2(-offset, offset),
				vec2(0.0, offset),
				vec2(offset, offset),
				vec2(-offset, 0.0),
				vec2(0.0, 0.0),
				vec2(offset, 0.0),
				vec2(-offset, -offset),
				vec2(0.0, -offset),
				vec2(offset, -offset)
			);

		float kernel[9] = float[]
		(
			1, 1, 1,
			1, -8, 1,
			1, 1, 1
		);

		vec3 sampleTex[9];
		for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(uScreenTexture, TexCoord.st + offsets[i]));
		}

		col = vec3(0.0);
		for(int i = 0; i < 9; i++)
		{
			col += sampleTex[i] * kernel[i];
		}
	}
	

	FragColor = vec4(col, 1.0);
}