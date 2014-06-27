
varying vec3 vertexNormal;
void main()
{
	//pass on tex coordinates
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_TexCoord[2] = gl_MultiTexCoord2;
	gl_TexCoord[3] = gl_MultiTexCoord3;

	vertexNormal = gl_NormalMatrix * gl_Normal;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}