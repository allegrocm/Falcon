/*
 *  LinearBlur.frag
 *  OSGEnhancer
 *
 *  Created by Kenneth Kopecky on 11/5/09.
 *  Copyright 2009 Orange Element Studios. All rights reserved.
 *
 */

//varying vec4 color;

uniform sampler2D glowInput;
uniform vec2 blurDirection;
uniform float blurDelta;
uniform float blurAmount;
uniform float glowGain;
//in practice, this is used to differentiate the shadow blender from the ambient occlusion blender
//uniform float depthRangeFactor;		//how much does the depth affect the blur range?
//uniform float AOBlendDepthFilter;
void main()
{
	vec2 texCoords = gl_TexCoord[0].xy;
	vec4 sum = vec4(0.0);
	vec4 infoHere = texture2D(glowInput, texCoords);
	float depthHere = infoHere.b;
	int minMax = int(blurAmount);//int(blendAmount / (depthHere * depthRangeFactor + 1.0));
	for(int x = -minMax; x <= minMax; x++)
	{
		vec2 coords = texCoords + blurDirection * blurDelta * float(x);
		float weight = 1.0 / (1.0 + abs(float(x) / (1.0 + float(blurAmount))));
		vec4 texData = texture2D(glowInput, coords);
		float depthThere = texData.b;
		float dDepth = abs(depthThere - depthHere);
	//		dDepth = 0.0;
//		float adjustedWeight = weight * (1.0 / (dDepth * AOBlendDepthFilter * 10.0 + 1.0));
//		weight = mix(weight, adjustedWeight, 1.0);
		sum.w += weight;
		sum.rgb += texData.rgb * weight;
	}
	sum.rgb /= sum.w;
	sum.rgb *= glowGain;
	vec4 colorHere = infoHere;
	//	sum.rgb = colorHere.rgb;
	gl_FragColor = vec4(sum.rgb, infoHere.a);
//	gl_FragColor.a = 0.25;
//	gl_FragColor.b = 1.0;
	//	gl_FragColor.r = 0.0;
	//	gl_FragColor.b = max(gl_FragColor.b, 0.0);
}
