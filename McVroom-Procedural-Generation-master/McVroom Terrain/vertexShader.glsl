#version 420 core

layout(location=0) in vec4 terrainCoords;
layout(location=1) in vec3 terrainNormals;
layout(location=2) in vec2 terrainTexCoords;

layout(location=3) in vec4 waterCoords;
layout(location=4) in vec3 waterNormals;
layout(location=5) in vec2 waterTexCoords;

layout(location=6) in vec4 cloudCoords;
layout(location=7) in vec3 cloudNormals;
layout(location=8) in vec2 cloudTexCoords;

layout(location=9) in vec3 skyboxCoords;

struct Material
{
vec4 ambRefl;
vec4 difRefl;
vec4 specRefl;
vec4 emitCols;
float shininess;
};

uniform vec4 globAmb;
uniform mat4 projMat;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat3 normalMat;
uniform uint object;
uniform float time;

uniform vec3 cameraPos;

out vec3 normalExport;
out vec2 texCoordsExport;

out vec3 skyboxTextures;

vec4 Coords;

void main(void)
{
	vec4 pos;
   if (object==0)
   {
		texCoordsExport = terrainTexCoords;
		normalExport = terrainNormals;
		normalExport = normalize(normalMat * normalExport);
		Coords = terrainCoords;
   }
   if (object==1)
   {
		pos = waterCoords;

		float amplitude = 1.;
		float frequency = 1.;
		pos.y = sin(pos.x * frequency);
		float t = 0.01*(time*130.0);
		pos.y += sin(pos.x*frequency*2.1 + t)*4.5;
		pos.y += sin(pos.x*frequency*1.72 + t*1.121)*4.0;
		pos.y += sin(pos.x*frequency*2.221 + t*0.437)*5.0;
		pos.y += sin(pos.x*frequency*3.1122+ t*4.269)*2.5;
		pos.y *= amplitude*0.06;

   		texCoordsExport = waterTexCoords;
		normalExport = waterNormals;
		normalExport = normalize(normalMat * normalExport);
		pos.y -= 3.0;
		Coords = pos;
   }
    if (object==2)
   {
		pos = cloudCoords;

	/*	float amplitude = 1.;
		float frequency = 1.;
		pos.y = sin(pos.x * frequency);
		float t = 0.01*(time*130.0);
		pos.y += sin(pos.x*frequency*2.1 + t)*4.5;
		pos.y += sin(pos.x*frequency*1.72 + t*1.121)*4.0;
		pos.y += sin(pos.x*frequency*2.221 + t*0.437)*5.0;
		pos.y += sin(pos.x*frequency*3.1122+ t*4.269)*2.5;
		pos.y *= amplitude*0.06;
		*/
   		texCoordsExport = cloudTexCoords;
		normalExport = cloudNormals;
		normalExport = normalize(normalMat * normalExport);
		Coords = pos;
   }
   if (object==3)
   {
		pos = projMat * viewMat * vec4(skyboxCoords.x + cameraPos.x , skyboxCoords.y + cameraPos.y , skyboxCoords.z + cameraPos.z , 1.0);
		gl_Position = pos.xyzw;
		skyboxTextures = vec3(skyboxCoords.x, skyboxCoords.y, skyboxCoords.z);

   } else
   {
		gl_Position = projMat * viewMat * modelMat * Coords;
   }
}