
//a simple per-vertex lighting shader, I hope
uniform sampler2D tex0;
uniform sampler2D tex1;
varying vec4 vertexColor;
uniform vec4 glowColor;			//can make this thing glow or pulse

void main()
{

	vec2 texCoords = gl_TexCoord[0].xy / 1.0;
//	texCoords = gl_FragCoord.xy / 200.0;
	vec4 mainTexture = texture2D(tex0, texCoords);
//	if(length(gl_TexCoord[0].rg) < 0.01)
//		mainTexture = vec4(1.0);

	vec4 finalColor = vertexColor;
	finalColor.a = clamp(finalColor.a, 0.0, 1.0);
	gl_FragColor = mainTexture * finalColor;
//	gl_FragColor.rgb = mix(gl_FragColor.rgb, glowColor.rgb, glowColor.a);
//	gl_FragColor.rgb += glowColor.rgb * glowColor.a;
	gl_FragColor = finalColor;
	gl_FragColor = mainTexture;
//	gl_FragColor.rgb += specColor.rgb * gl_FragColor.a;
//	gl_FragColor.rgb = vec3(abs(dot(normalize(gl_LightSource[0].position.xyz), surfaceNormal)));
//	gl_FragColor.rg = texCoords;
}