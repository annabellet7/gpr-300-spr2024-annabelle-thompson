/*References:
 *https://youtu.be/uihBwtPIBxM?si=vACYAby7_A3yxnZH
 *https://en.wikipedia.org/wiki/Sobel_operator
 *https://docs.opencv.org/4.x/d4/d86/group__imgproc__filter.html#gacea54f142e81b6758cb6f375ce782c8d
 *https://www.youtube.com/watch?v=LDhN-JK3U9g&t=975s
 */

#version 450

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D uScreenTexture;

uniform bool uEdgeOn;
uniform float uEdge;
uniform float uThreshold;

//return the variance in hsv color code or the "brightness"
//of the color
float brightness (vec3	color)
{
	return max(color.r, max(color.g, color.b));
}

void main()
{

	vec3 col = texture(uScreenTexture, TexCoord).rgb;

	float x = 0.0;
	float y = 0.0;

	if (uEdgeOn)
	{
		//determines the texel size
		float offset = 1.0 / uEdge;
		float edge;

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
	
		//difference between pixels in the x direction
		float gX[9] = float[]
		(
			-1, 0, 1,
			-2, 0, 2,
			-1, 0, 1
		);

		//difference between pixels in the y direction
		float gY[9] = float[]
		(
			-1, -2, -1,
			 0,  0,  0,
			 1,  2,  1
		);

		//sample texture and get varience
		float sampleTex[9];
		for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = brightness(vec3(texture(uScreenTexture, TexCoord.st + offsets[i])));
		}

		//find the difference between x and y pixels and total them
		for(int i = 0; i < 9; i++)
		{
			x += sampleTex[i] * gX[i];
			y += sampleTex[i] * gY[i];
		}

		//magnitude of the gradient
		float magnitude = sqrt(x * x + y * y);
		edge = magnitude;

		//determine if value counts as edge
		if (magnitude < uThreshold)
		{
			edge = 0.0f;
		}

		//subtracts the edge color from the original texture
		col = vec3(col - vec3(edge));
	}	

    FragColor = vec4(col, 1.0);
}