/* References: 
 *https://www.kyprianidis.com/p/pg2009/jkyprian-pg2009.pdf
 *https://www.kyprianidis.com/p/npar2011/jkyprian-npar2011.pdf
 *https://www.umsl.edu/~kangh/Papers/kang-tpcg2010.pdf
 *https://www.cs.cmu.edu/~sarsen/structureTensorTutorial/
 *https://en.wikipedia.org/wiki/Kuwahara_filter
 *https://medium.com/swlh/what-is-a-kuwahara-filter-77921ce286f2
 *https://github.com/GarrettGunnell/Post-Processing/blob/main/Assets/Kuwahara%20Filter/AnisotropicKuwahara.shader
 *https://www.youtube.com/watch?v=LDhN-JK3U9g&t=975s
 *Blender source code
 */

#version 450

out vec4 FragColor;

in vec4 StructureTensor;
in vec2 TexCoord;

uniform sampler2D uScreenTexture;

uniform bool uBKuwaharaOn;
uniform int uBRadius;


uniform bool uAKuwaharaOn;
uniform int uARadius;
uniform int uSectors;
uniform float uEccentricity;
uniform float uSharpness;

uniform float uMix;


#define M_PI 3.1415926535897932384626433832795
#define M_SQRT1_2 0.70710678118654752440 /* 1/sqrt(2) */

//return the variance in hsv color code or the "brightness"
//of the color
float brightness (vec3	color)
{
	return max(color.r, max(color.g, color.b));
}


void main()
{

	vec4 col = texture(uScreenTexture, TexCoord).rgba;
	vec3 bKuwaharaCol = vec3(0.0);
	vec3 aKuwaharaCol = vec3(0.0);

	//if both kuwahara filters have a radius of 0 no chance is made
	if (uARadius == 0 && uBRadius == 0)
	{
		FragColor = vec4(col.rgb, 1.0);
	}

	//checks if basic kuwahara filter is on and if the radius is not zero
	if (uBKuwaharaOn && uBRadius != 0)
	{
		vec4 colors[4] = {vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)};
		vec4 total[4] = {vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)};
		vec4 sqBrightness = vec4(0.0);

		int sideLength = uBRadius + 1;
		int numSamples = sideLength * sideLength;

		vec2 texelSize = 1.0 / textureSize(uScreenTexture, 0).xy;

		//gets the top left corner of each quadrant 
		vec2[4] quadrants = {TexCoord.st, 
							 TexCoord.st + vec2(-uBRadius * texelSize.x, 0.0), 
							 TexCoord.st + vec2(-uBRadius, -uBRadius) * texelSize, 
							 TexCoord.st + vec2(0.0, -uBRadius * texelSize.y)};

		//gets the color for each texel in each quadrant
		//also gets the brightness for each texel
		for (int i = 0; i < sideLength; i++)
		{
			for (int j = 0; j < sideLength; j++)
			{
				vec2 xy = vec2(j, i) * texelSize;
				vec4 bright;

				//quad one
				colors[0].rgb = texture(uScreenTexture, (quadrants[0] + xy)).rgb;
				colors[0].a = brightness(colors[0].rgb);

				//quad two
				colors[1].rgb = texture(uScreenTexture, (quadrants[1] + xy)).rgb;
				colors[1].a = brightness(colors[1].rgb);

				//quad three
				colors[2].rgb = texture(uScreenTexture, (quadrants[2] + xy)).rgb;
				colors[2].a = brightness(colors[2].rgb);

				//quad four
				colors[3].rgb = texture(uScreenTexture, (quadrants[3] + xy)).rgb;
				colors[3].a = brightness(colors[3].rgb);

				total[0] += colors[0];
				total[1] += colors[1];
				total[2] += colors[2];
				total[3] += colors[3];

				bright = vec4(colors[0].a, colors[1].a, colors[2].a, colors[3].a);

				sqBrightness += bright * bright;
			}
		}

		//finds the standard deviation of each quadrant
		vec4 bright = vec4(total[0].a, total[1].a, total[2].a, total[3].a);
		vec4 deviation = sqBrightness / float(numSamples) - (bright * bright) / float(numSamples * numSamples);
		
		//checks which quad has the lowest standard deviation
		int index = deviation[0] < deviation[1] ? 0 : 1;
		index = deviation[index] < deviation[2] ? index : 2;
		index = deviation[index] < deviation[3] ? index : 3;

		//sets the center pixel color to the average color of the quadrants
		//with the lowest standard deviation
		bKuwaharaCol = total[index].rgb / float(numSamples);
	}

	//------------------------------------------------------------------------------

	//checks of anisotropic kuwahara filter is on and radius is not 0
	if (uAKuwaharaOn && uARadius != 0)
	{
		//use sobel to get the structor tensor of the image
		vec2 texelSize = 1.0 / textureSize(uScreenTexture, 0).xy;
		float x = 0.0;
		float y = 0.0;

		//use texel size to create offsets
		vec2 offsets[9] = vec2[] 
		(
			vec2(-texelSize.x, texelSize.y),
			vec2(0.0, texelSize.y),
			vec2(texelSize.x, texelSize.y),
			vec2(-texelSize.x, 0.0),
			vec2(0.0, 0.0),
			vec2(texelSize.x, 0.0),
			vec2(-texelSize.x, -texelSize.y),
			vec2(0.0, -texelSize.y),
			vec2(texelSize.x, -texelSize.y)
		);

		//sobel kernel for x direction
		float gX[9] = float[]
		(
			-1, 0, 1,
			-2, 0, 2,
			-1, 0, 1
		);

		//sobel kernel for y direction
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
		//approximation of partial derivatives
		for(int i = 0; i < 9; i++)
		{
			x += sampleTex[i] * gX[i];
			y += sampleTex[i] * gY[i];
		}


		//------------------------------------------------------------------------------

		//structure tensor
		float dxdx = x*x;
		float dxdy = x*y;
		float dydy = y*y;
		
		//get eigen values
		float eigenValue = (dxdx + dydy) / 2.0;
		float eigenValueSqrt = sqrt((dxdx - dydy) * (dxdx - dydy) + 4.0 * (dxdy * dxdy)) / 2.0;

		float firstEigen = eigenValue + eigenValueSqrt;
		float secondEigen = eigenValue - eigenValueSqrt;

		//get eigen vector and normalize to get unit eigen vector
		//getting the unit vector avoids additional trig later
		vec2 eigenVector = vec2(firstEigen - dxdx, -dxdy);
		float eigenVectorLength = length(eigenVector);
		vec2 unitEigenVector = eigenVectorLength != 0.0 ? eigenVector / eigenVectorLength : vec2(1.0);

		//get anisotropy, ranges 0 to 1 where zero is isotropic and 1 is completely anisotropic
		float eigenValueSum = firstEigen + secondEigen;
		float eigenValueDifference = firstEigen - secondEigen;
		float anisotropy = eigenValueSum != 0.0 ? eigenValueDifference / eigenValueSum : 0.0;

		//------------------------------------------------------------------------------

		//get height and width of ellipse, high anisotropy will have a more stretched ellipse
		//and low anisotropy will be closer to a circle. 
		float radius = uARadius;
		float width = radius * clamp((uEccentricity + anisotropy) / uEccentricity, 0.1, 2.0); 
		float height = radius * clamp(uEccentricity / (uEccentricity + anisotropy), 0.1, 2.0); 

		//angle eigen vector makes with x axis
		float cosEigen = unitEigenVector.x;
		float sinEigen = unitEigenVector.y;

		//inverse transformation maxtrix to transform ellipse into unit circle
		mat2 inverseEllipse = mat2(cosEigen / width, -sinEigen / height,
								   sinEigen / width,  cosEigen / height);

		//get the bounding box of the ellipse
		vec2 majorAxis = width * unitEigenVector;
		vec2 minorAxis = height * unitEigenVector * vec2(-1, 1);
		vec2 boundingBox = vec2(ceil(sqrt((majorAxis * majorAxis) + (minorAxis * minorAxis))));

		//values for polynomial weighting
		float sectorOverlap = 2.0 / radius;
		float sectorEvelopeAngle = ((3.0 / 2.0) * M_PI) / uSectors;
		float crossSectorOverlap = (sectorOverlap + cos(sectorEvelopeAngle)) / 
							   (sin(sectorEvelopeAngle) * sin(sectorEvelopeAngle));

		vec4 weightMeanSquaredSectorColor[8];
		vec4 weightMeanSectorColor[8];
		float sectorWeightsSum[8];

		//center color not included in loop
		//(0,0) coods cancel out most expensive computations 
		vec4 centerColSq = col * col;
		float centerWeight = 1.0 / uSectors;
		vec4 weightCenterColor = col * centerWeight;
		vec4 weightCenterColorSq = centerColSq * centerWeight;
		for (int i = 0; i < uSectors; i++)
		{
			weightMeanSectorColor[i] = weightCenterColor;
			weightMeanSquaredSectorColor[i] = weightCenterColorSq;
			sectorWeightsSum[i] =  centerWeight;
		}

		//loop over pixels in bounding box, use ellipise symmetry to optimze and compute
		//two mirrored pixels at the same time using the same weight
		for (int j = 0; j <= int(boundingBox.y); ++j)
		{
			for (int i = int(-boundingBox.x); i <= int(boundingBox.x); ++i)
			{
				//skip negative x pixels due to mirroring because they will
				//be the same as positive x pixels when y is 0, also skip the 
				//center pixel so it does not mirror itself
				//use of conditional because it is faster to not calculate the 
				//unused pixels than to throw them away
				if(j == 0 && i <= 0)
				{
					continue;
				}

				//check if point is in the ellipse
				//same use of conditional
				vec2 diskPoint = inverseEllipse * vec2(i, j);
				float diskPointLengthSq = dot(diskPoint, diskPoint);
				if (diskPointLengthSq > 1.0)
				{
					continue;
				}
			
				float sectorWeights[8];
			
				//compute every other index with 90 degree rotations, using that a 90 degree rotation
				//consists of swapping the coordinates and making the x value negative
				vec2 polynomial = sectorOverlap - crossSectorOverlap * (diskPoint * diskPoint);
				sectorWeights[0] = max(0.0, diskPoint.y + polynomial.x) * max(0.0, diskPoint.y + polynomial.x);
				sectorWeights[2] = max(0.0, -diskPoint.x + polynomial.y) * max(0.0, -diskPoint.x + polynomial.y);
				sectorWeights[4] = max(0.0, -diskPoint.y + polynomial.x) * max(0.0, -diskPoint.y + polynomial.x);
				sectorWeights[6] = max(0.0, diskPoint.x + polynomial.y) * max(0.0, diskPoint.x + polynomial.y);

				//rotate disk by 45 degrees using constants
				vec2 rotateDisk = M_SQRT1_2 * vec2(diskPoint.x - diskPoint.y, diskPoint.x + diskPoint.y);

				//use rotated disk to caclulate rotated polynomial and remaining sectors
				vec2 rotatePolynomial = sectorOverlap - crossSectorOverlap * (rotateDisk * rotateDisk);
				sectorWeights[1] = max(0.0, rotateDisk.y + rotatePolynomial.x) * max(0.0, rotateDisk.y + rotatePolynomial.x);
				sectorWeights[3] = max(0.0, -rotateDisk.x + rotatePolynomial.y) * max(0.0, -rotateDisk.x + rotatePolynomial.y);
				sectorWeights[5] = max(0.0, -rotateDisk.y + rotatePolynomial.x) * max(0.0, -rotateDisk.y + rotatePolynomial.x);
				sectorWeights[7] = max(0.0, rotateDisk.x + rotatePolynomial.y) * max(0.0, rotateDisk.x + rotatePolynomial.y);

				float sum = 0.0;
				for (int k = 0; k < uSectors; k++)
				{
					sum += sectorWeights[k];
				}

				//calculate radial Gaussian weighting so pixels further away from the center
				//have less weight, normalize by dividing by the sum of the weights
				float radialGaussianWeight = exp(-M_PI * diskPointLengthSq) / sum; //expensive
			
				//get pixel and its mirror
				vec4 upperCol = texture(uScreenTexture, TexCoord + vec2(i, j) * texelSize);
				vec4 lowerCol = texture(uScreenTexture, TexCoord - vec2(i, j) * texelSize);
				vec4 upperColSq = upperCol * upperCol;
				vec4 lowerColSq = lowerCol * lowerCol;

				//sum the weighted color of each sector
				for (int k = 0; k < uSectors; ++k)
				{
					float weight = sectorWeights[k] * radialGaussianWeight;

					int upperIndex = k;
					sectorWeightsSum[upperIndex] += weight;
					weightMeanSectorColor[upperIndex] += upperCol * weight;
					weightMeanSquaredSectorColor[upperIndex] += upperColSq * weight;

					int lowerIndex = (k + uSectors / 2) % uSectors;
					sectorWeightsSum[lowerIndex] += weight;
					weightMeanSectorColor[lowerIndex] += lowerCol * weight;
					weightMeanSquaredSectorColor[lowerIndex] += lowerColSq * weight;
				}		
			}
		}

		//get average weighted sum of sectors, sectors with lower deviation have more
		//weight than sectors with a higher deviation
		float sumWeights = 0.0;
		vec4 weightedSum = vec4(0.0);
		for (int i = 0; i < uSectors; i++)
		{
			weightMeanSectorColor[i] /= sectorWeightsSum[i];
			weightMeanSquaredSectorColor[i] /= sectorWeightsSum[i];

			vec4 colorMean = weightMeanSectorColor[i];
			vec4 colorMeanSq = weightMeanSquaredSectorColor[i];
			vec4 colorVar = abs(colorMeanSq - colorMean * colorMean);
	
			float deviation = dot(sqrt(colorVar.rgb), vec3(1.0));
			
			//get sector weight, use 0.02 to avoid 0 division and avoid artifacts
			float weight = 1.0 / pow(max(0.02, deviation), uSharpness);

			sumWeights += weight;
			weightedSum += colorMean * weight;
		}

		weightedSum = sumWeights != 0.0 ? weightedSum / sumWeights : col;
		aKuwaharaCol = weightedSum.rgb;
	}	

	//if both filters are on mix the output
	if (uBKuwaharaOn == true && uAKuwaharaOn)
	{
		col.rgb = mix(bKuwaharaCol, aKuwaharaCol, uMix);
	}
	else if (uBKuwaharaOn)
	{
		col.rgb = bKuwaharaCol;
	}
	else if (uAKuwaharaOn)
	{
		col.rgb = aKuwaharaCol;
	}
	FragColor = vec4(col.rgb, 1.0);
}