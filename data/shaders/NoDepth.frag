
uniform sampler2D Tex;

void main()
{
	vec4 tex = texture2D(Tex, gl_TexCoord[0].xy);
	gl_FragColor = tex;
	gl_FragColor.b = 0.5;
	gl_FragDepth = 0.999;
}