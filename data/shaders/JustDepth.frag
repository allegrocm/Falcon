//this shader draws black. intended to be like just drawing to the depth buffer

uniform vec4 glowColor;		//black by default
void main()
{

	gl_FragColor = glowColor * glowColor.a;
	
}