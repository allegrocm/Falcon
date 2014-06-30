
//a simple per-pixel lighting shader, I hope
uniform sampler2D tex0;
uniform sampler2D tex1;
varying vec3 vertexNormal;

vec4 ambientColor;
vec4 diffuseColor;
vec4 specColor;

vec3 surfaceNormal;		//includes bump/normal map, if applicable

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
		vec3 lightReflect = reflect(gl_LightSource[i].position.rgb * -1.0, surfaceNormal);
		float specEx = gl_FrontMaterial.shininess;		//specular exponent
		float specAmount = lightReflect.z;
		specColor.rgb =+ vec3(specAmount);
	}
		ambientColor *= gl_FrontMaterial.ambient;
		specColor *= gl_FrontMaterial.specular;
		diffuseColor *= gl_FrontMaterial.diffuse;
	
}

void main()
{

	vec3 lerpedNormal = normalize(vertexNormal);		//interpolated normal
	surfaceNormal = lerpedNormal;
	vec2 texCoords = gl_TexCoord[0].xy;
//	texCoords = gl_FragCoord.xy / 200.0;
	vec4 mainTexture = texture2D(tex0, texCoords);
//	if(length(gl_TexCoord[0].rg) < 0.01)
//		mainTexture = vec4(1.0);
	calcLighting();
	vec4 finalColor = ambientColor + diffuseColor;
	finalColor.a = clamp(finalColor.a, 0.0, 1.0);
	gl_FragColor = mainTexture * finalColor;

//	gl_FragColor.rgb += specColor.rgb * gl_FragColor.a;
//	gl_FragColor.rgb = vec3(abs(dot(normalize(gl_LightSource[0].position.xyz), surfaceNormal)));
//	gl_FragColor.rg = texCoords;
}