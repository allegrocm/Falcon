

varying vec4 projCoords;

//these are actually in...view space or something
varying vec3 oNormal;
varying vec3 owsTangent;		//original worldspace tangent
varying vec3 owsBinormal;		//original worldspace binormal
varying vec3 owsNormal;			//original worldspace normal

varying vec4 vert;
void main()
{
	vert = gl_Vertex;
	projCoords = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_TexCoord[2] = gl_MultiTexCoord2;
	gl_TexCoord[3] = gl_MultiTexCoord3;

	
	//calculate the tangent and binormal
	vec4 tang = gl_MultiTexCoord2;
	vec4 bin = gl_MultiTexCoord3;
	vec4 norm;
	norm.xyz = gl_NormalMatrix * gl_Normal;
	norm.w = 0.0;

	owsNormal = norm.xyz;
	norm.w = 0.0;
	tang.w = 0.0;
	bin.w = 0.0;
	tang.xyz = gl_NormalMatrix * tang.xyz;

	bin.xyz = gl_NormalMatrix * bin.xyz;
//	tang = gl_ModelViewMatrixInverse * tang;
//	bin = gl_ModelViewMatrixInverse * bin;
//	norm = gl_ModelViewMatrixInverse * norm;
	owsTangent = tang.xyz;
	owsBinormal = bin.xyz;
//	owsNormal = gl_Normal;
	gl_Position = projCoords;
	oNormal = normalize(gl_Normal);
	
}
