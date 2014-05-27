uniform sampler2D glowInput;


void main()
{
	
	vec4 glowMap = texture2D(glowInput, gl_TexCoord[0].xy);
	gl_FragColor = glowMap.rgga;

}