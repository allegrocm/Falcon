


varying vec4 projCoords;
uniform mat4 sprayCamMVP;
uniform mat4 camModelViewInverse;
uniform mat4 XformInverse;			//inverse transform of this particular model
uniform mat4 Xform;
uniform vec3 eyePos;		//worldspace
uniform sampler2D tex0;		//main texture is also our bump map :-/
uniform sampler2D tex1;
varying vec3 oNormal;
varying vec4 vert;
varying vec3 owsTangent;		//original worldspace tangent
varying vec3 owsBinormal;		//original worldspace binormal
varying vec3 owsNormal;			//original worldspace normal

vec3 wsTangent;
vec3 wsBinormal;

vec3 normal;
vec3 tsNormal;		//as read from the normal map
vec3 wsMappedNormal;		//calculated from the normal map
vec3 wsNormal;
vec3 WSPos;
vec3 tangent;
vec3 binormal;

vec4 diffuseColor;
vec3 diffuseLight;
vec3 ambientLight;
vec4 ambientColor;
vec4 specColor;
vec4 finalColor;
vec3 specularLight;

vec3 surfaceNormal;

void calcNormal()
{
	float dTex = 1.0 / 512.0;
//	vec4 here = texture2D(tex0, gl_TexCoord[0].xy);
	vec2 BMCoords = gl_TexCoord[0].xy * 4.0;
	vec4 xMinus = texture2D(tex1, BMCoords - vec2(dTex, 0.0));
	vec4 xPlus = texture2D(tex1, BMCoords + vec2(dTex, 0.0));
	vec4 zMinus = texture2D(tex1, BMCoords - vec2(0.0, dTex));
	vec4 zPlus = texture2D(tex1, BMCoords + vec2(0.0, dTex));
	
	//come up with a tangent space normal first
	float bumpScale = 5.0;
	vec3 TSNormal = vec3(xPlus.r-xMinus.r, zPlus.r-zMinus.r, 1.0 / bumpScale);
	TSNormal = normalize(TSNormal);
//	TSNormal = vec3(0.0, 0.0, 1.0);
//	surfaceNormal = TSNormal;
//	surfaceNormal = normalize(owsNormal);
	surfaceNormal = normalize(owsTangent) * TSNormal.x + normalize(owsNormal) * TSNormal.z + normalize(owsBinormal) * TSNormal.y;
}

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
//	ambientColor *= gl_FrontMaterial.ambient;
//	specColor *= gl_FrontMaterial.specular;
//	diffuseColor *= gl_FrontMaterial.diffuse;
	
}


void main()
{
	calcNormal();
	vec2 texCoords = gl_TexCoord[0].xy;
//	texCoords = gl_FragCoord.xy / 200.0;
	vec4 mainTexture = texture2D(tex0, texCoords);
//	if(length(gl_TexCoord[0].rg) < 0.01)
//		mainTexture = vec4(1.0);
	calcLighting();
	vec4 finalColor = ambientColor + diffuseColor;
	finalColor.rgb += gl_FrontMaterial.emission.rgb;
	finalColor.a = clamp(finalColor.a, 0.0, 1.0);
	gl_FragColor = finalColor;
//	gl_FragColor = mainTexture * finalColor;
//	gl_FragColor.rgb = surfaceNormal * 0.5 + 0.5;
//	if(gl_FragCoord.x > 700.0)
//		gl_FragColor.rgb = mainTexture.rgb;
}
