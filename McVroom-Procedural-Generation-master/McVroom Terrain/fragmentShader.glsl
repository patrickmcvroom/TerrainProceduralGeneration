#version 430 core

in vec3 normalExport;
in vec2 texCoordsExport;
in vec3 skyboxTextures;
out vec4 colorsExport;
out vec4 fieldTexColor;
out vec4 waterTexColor;
out vec4 cloudTexColor;

struct Light
{
   vec4 ambCols;
   vec4 difCols;
   vec4 specCols;
   vec4 coords;
};

uniform Light light0;

uniform vec4 globAmb;
  
struct Material
{
   vec4 ambRefl;
   vec4 difRefl;
   vec4 specRefl;
   vec4 emitCols;
   float shininess;
};

uniform uint object;

uniform Material terrainFandB;
uniform Material waterFandB;
uniform Material cloudFandB;

uniform sampler2D grassTex;
uniform sampler2D waterTex;
uniform sampler2D cloudTex;
uniform samplerCube skybox;

vec3 normal, lightDirection;
vec4 fAndBDif;

void main(void)
{
	vec4 colorsOut;
	if(object == 0)
	{
	   normal = normalize(normalExport);
	   lightDirection = normalize(vec3(light0.coords));
	   fAndBDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols *
	   terrainFandB.difRefl);
	   fieldTexColor = texture(grassTex, texCoordsExport);
	   colorsOut = fAndBDif * fieldTexColor;
	     colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * colorsOut;
   }
   	if(object == 1)
	{
	   normal = normalize(normalExport);
	   lightDirection = normalize(vec3(light0.coords));
	   fAndBDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * waterFandB.difRefl);
	   waterTexColor = texture(waterTex, texCoordsExport);
	   colorsOut = fAndBDif * waterTexColor;
	  // colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * colorsOut;
	    colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * colorsOut;
   }
   if(object == 2)
   {
		 normal = normalize(normalExport);
	   lightDirection = normalize(vec3(light0.coords));
	   fAndBDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * cloudFandB.difRefl);
	   cloudTexColor = texture(cloudTex, texCoordsExport);
	   colorsOut = fAndBDif * cloudTexColor;
	   //colorsExport = vec4(1,0,1,1);
	   //colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * colorsOut;
	     colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * colorsOut;
   } 
   if(object == 3)
   {
		colorsExport = texture(skybox, skyboxTextures);
		//colorsExport = vec4(1,0,1,1);
   } 

	 //colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * colorsOut;
   
   //colorsExport = vec4(1,0,1,1);
  // colorsExport =  vec4(vec3(min(fAndBDif, vec4(1.0))), 1.0) * colorsOut;
}