
varying vec4 vertexColor;

vec4 ambientColor;
vec4 diffuseColor;
vec4 specColor;

vec3 surfaceNormal;		//includes bump/normal map, if applicable

vec3 vertexNormal;

void calcLighting()
{
	ambientColor = gl_LightModel.ambient;
	diffuseColor = vec4(0.0);
	specColor = vec4(0.0);
	for(int i = 0; i < 2; i++)
	{
		ambientColor += gl_LightSource[i].ambient;
		float lightDotSurface = clamp(dot(normalize(gl_LightSource[i].position.xyz), surfaceNormal), 0.0, 1.0);
		diffuseColor += gl_LightSource[i].diffuse * lightDotSurface;
		vec3 lightReflect = normalize(reflect(gl_LightSource[i].position.rgb * -1.0, surfaceNormal));
		float specEx = gl_FrontMaterial.shininess;		//specular exponent
		float specAmount = lightReflect.z;
		specColor.rgb =+ vec3(specAmount);
	}
//	ambientColor *= gl_FrontMaterial.ambient;
//	specColor *= gl_FrontMaterial.specular;
//	diffuseColor *= gl_FrontMaterial.diffuse;
	vertexColor = ambientColor * gl_FrontMaterial.ambient;
	vertexColor += specColor * gl_FrontMaterial.specular+ diffuseColor * gl_FrontMaterial.diffuse;

}



void main()
{
	//pass on tex coordinates
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_TexCoord[2] = gl_MultiTexCoord2;
	gl_TexCoord[3] = gl_MultiTexCoord3;

	vertexNormal = normalize(gl_NormalMatrix * gl_Normal);
	surfaceNormal = vertexNormal;
	calcLighting();
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}