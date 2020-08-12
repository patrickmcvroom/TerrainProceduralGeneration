#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <GL/glew.h>
#include <GL/freeglut.h>

#include <time.h>
#include <stdlib.h>
#include <vector>
#include "getbmp.h"
#include "getbmp.h"
#include "SOIL/SOIL.h"

//#include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 

using namespace std;
using namespace glm;

// Size of the terrain
const int MAP_SIZE = 129;
// Size of the water
const int OCEAN_SIZE = 129;

const int CLOUD_SIZE = 129;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

vector<std::string> skyboxNames;
unsigned int skyboxMap, skyboxVAO, skyboxVBO;

// Create camera
//Camera camera;

static mat4 modelMat = mat4(1.0);
static mat4 viewMat = mat4(1.0);

struct Vertex
{
	/*float coords[4];
	float colors[4];*/

	glm::vec4 coords;
	glm::vec3 normals;
	glm::vec2 texcoords;
};

struct Matrix4x4
{
	float entries[16];
};

// GLOBAL AMBIENT LIGHT
static const vec4 globAmb = vec4(0.2, 0.2, 0.2, 1.0);

struct Light
{
	vec4 ambCols;
	vec4 difCols;
	vec4 specCols;
	vec4 coords;
};

static const Light light0 =
{
vec4(0.0, 0.0, 0.0, 1.0),
vec4(1.0, 1.0, 1.0, 1.0),
vec4(1.0, 1.0, 1.0, 1.0),
vec4(1.0, 1.0, 0.0, 0.0)
};

// MATERIAL PROPERTIES
struct Material
{
	vec4 ambRefl;
	vec4 difRefl;
	vec4 specRefl;
	vec4 emitCols;
	float shininess;
};

// SET TERRAIN PROPERTIES
static const Material terrainFandB =
{
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	50.0f
};

// SET WATER PROPERTIES
static const Material waterFandB =
{
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	50.0f
};

// SET CLOUD PROPERTIES
static const Material cloudFandB =
{
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	50.0f
};



static mat4 projMat = mat4(1.0);

// static mat3 normalMat = mat3(1.0);

static const Matrix4x4 IDENTITY_MATRIX4x4 =
{
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	}
};

glm::vec3 skyboxVertices[] = {
	// positions          
	glm::vec3 (-100.0f,  100.0f, -100.0f),
	glm::vec3(-100.0f, -100.0f, -100.0f),
	glm::vec3(100.0f, -100.0f, -100.0f),
	glm::vec3(100.0f, -100.0f, -100.0f),
	glm::vec3(100.0f,  100.0f, -100.0f),
	glm::vec3(-100.0f,  100.0f, -100.0f),

	glm::vec3(-100.0f, -100.0f,  100.0f),
	glm::vec3(-100.0f, -100.0f, -100.0f),
	glm::vec3(-100.0f,  100.0f, -100.0f),
	glm::vec3(-100.0f,  100.0f, -100.0f),
	glm::vec3(-100.0f,  100.0f,  100.0f),
	glm::vec3(-100.0f, -100.0f,  100.0f),

	glm::vec3(100.0f, -100.0f, -100.0f),
	glm::vec3(100.0f, -100.0f,  100.0f),
	glm::vec3(100.0f,  100.0f,  100.0f),
	glm::vec3(100.0f,  100.0f,  100.0f),
	glm::vec3(100.0f,  100.0f, -100.0f),
	glm::vec3(100.0f, -100.0f, -100.0f),

	glm::vec3(-100.0f, -100.0f,  100.0f),
	glm::vec3(-100.0f,  100.0f,  100.0f),
	glm::vec3(100.0f,  100.0f,  100.0f),
	 glm::vec3(100.0f,  100.0f,  100.0f),
	glm::vec3(100.0f, -100.0f,  100.0f),
	glm::vec3(-100.0f, -100.0f,  100.0f),

	glm::vec3(-100.0f,  100.0f, -100.0f),
	glm::vec3(100.0f,  100.0f, -100.0f),
	glm::vec3(100.0f,  100.0f,  100.0f),
	glm::vec3(100.0f,  100.0f,  100.0f),
	glm::vec3(-100.0f,  100.0f,  100.0f),
	glm::vec3(-100.0f,  100.0f, -100.0f),

	glm::vec3(-100.0f, -100.0f, -100.0f),
	glm::vec3(-100.0f, -100.0f,  100.0f),
	glm::vec3(100.0f, -100.0f, -100.0f),
	glm::vec3(100.0f, -100.0f, -100.0f),
	glm::vec3(-100.0f, -100.0f,  100.0f),
	glm::vec3(100.0f, -100.0f,  100.0f)
};



static enum buffer { TERRAIN_VERTICES, WATER_VERTICES, CLOUD_VERTICES };
static enum object { TERRAIN, WATER, CLOUDS };

// Globals
// Terrain and water and cloud vertices
static Vertex terrainVertices[MAP_SIZE * MAP_SIZE] = {};
static Vertex waterVertices[OCEAN_SIZE * OCEAN_SIZE] = {};
static Vertex cloudVertices[CLOUD_SIZE * CLOUD_SIZE] = {};

static BitMapFile* image[3]; // local storage for image BMP data CHANGE THESE FOR ADDING TEXTURES
static unsigned int
texture[3],
grassTexLoc,
waterTexLoc,
cloudTexLoc;

// Terrain and water strips
const int terrainNumStripsRequired = MAP_SIZE - 1;
const int terrainVerticesPerStrip = 2 * MAP_SIZE;

const int waterNumStripsRequired = OCEAN_SIZE - 1;
const int waterVerticesPerStrip = 2 * OCEAN_SIZE;

const int cloudNumStripsRequired = CLOUD_SIZE - 1;
const int cloudVerticesPerStrip = 2 * CLOUD_SIZE;

// Terrain and water index data
unsigned int terrainIndexData[terrainNumStripsRequired][terrainVerticesPerStrip];

unsigned int waterIndexData[waterNumStripsRequired][waterVerticesPerStrip];

unsigned int cloudIndexData[cloudNumStripsRequired][cloudVerticesPerStrip];

static unsigned int
programId,
vertexShaderId,
fragmentShaderId,
modelMatLoc, // viewMat, modelMat
viewMatLoc,
projMatLoc,
normalMatLoc,
buffer[10],
vao[10];

// Function to read text file, used to read shader files
char* readTextFile(char* aTextFile)
{
	FILE* filePointer = fopen(aTextFile, "rb");
	char* content = NULL;
	long numVal = 0;

	fseek(filePointer, 0L, SEEK_END);
	numVal = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	content = (char*)malloc((numVal + 1) * sizeof(char));
	fread(content, 1, numVal, filePointer);
	content[numVal] = '\0';
	fclose(filePointer);
	return content;
}

void shaderCompileTest(GLuint shader)
{
	GLint result = GL_FALSE;
	int logLength;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
	std::cout << &vertShaderError[0] << std::endl;
}


unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

// Initialization routine.
void setup(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// step size

	int terrainStepSize = MAP_SIZE - 1;

	// Initialise terrain - set values in the height map to 0
	float terrain[MAP_SIZE][MAP_SIZE] = {};
	for (int x = 0; x < MAP_SIZE; x++)
	{
		for (int z = 0; z < MAP_SIZE; z++)
		{
			terrain[x][z] = 0;
		}
	}

	// Initialise ocean - set values in the height map to 0
	float water[OCEAN_SIZE][OCEAN_SIZE] = {};
	for (int x = 0; x < OCEAN_SIZE; x++)
	{
		for (int z = 0; z < OCEAN_SIZE; z++)
		{
			water[x][z] = 0.0f;
		}
	}

	// Initialise cloud - set values in the height map to 10
	float cloud[CLOUD_SIZE][CLOUD_SIZE] = {};
	for (int x = 0; x < CLOUD_SIZE; x++)
	{
		for (int z = 0; z < CLOUD_SIZE; z++)
		{
			cloud[x][z] = 50.0f;
		}
	}

	// TODO: Add your code here to calculate the height values of the terrain using the Diamond square algorithm

	srand(time(NULL));

	float height = 9.0f; // -min + rand() / ( RAND_MAX / min + min);

	// static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (height + height));

	terrain[0][0] = (rand()%10/5.0-1.0)* height;
	terrain[0][MAP_SIZE - 1] = (rand() % 10 / 5.0 - 1.0) * height;
	terrain[MAP_SIZE - 1][0] = (rand() % 10 / 5.0 - 1.0) * height;
	terrain[MAP_SIZE - 1][MAP_SIZE - 1] = (rand() % 10 / 5.0 - 1.0) * height;

	int a, b, count;

	float p1, p2, p3, p4, average;

	// DIAMOND STEP
	int max = 10;

	while (terrainStepSize > 1)
	{
		for (int a = 0; a < MAP_SIZE - 1; a += terrainStepSize)
		{
			for (int b = 0; b < MAP_SIZE - 1; b += terrainStepSize)
			{
				p1 = terrain[a][b];
				p2 = terrain[a+terrainStepSize][b];
				p3 = terrain[a][b+terrainStepSize];
				p4 = terrain[a + terrainStepSize][a + terrainStepSize];

				average = ((p1 + p2 + p3 + p4) / 4) + (rand() % max / 5.0 - 1.0) * height;
				terrain[a + terrainStepSize / 2][b + terrainStepSize / 2] = average;
			}
		}

		for (int a = 0; a < MAP_SIZE - 1; a += terrainStepSize)
		{
			for (int b = 0; b < MAP_SIZE - 1; b += terrainStepSize)
			{

				// SQUARE STEP PART 1

				count = 0;
				p1 = terrain[a][b];
				count++;
				p2 = terrain[a + terrainStepSize][b];
				count++;

				if ((a - terrainStepSize / 2) >= 0)
				{
					p3 = terrain[a - terrainStepSize / 2][b + terrainStepSize / 2];
					count++;
				}
				else
				{
					p3 = 0;
				}

				if ((a + terrainStepSize / 2) < MAP_SIZE)
				{
					p4 = terrain[a + terrainStepSize / 2][b + terrainStepSize / 2];
					count++;
				}
				else
				{
					p4 = 0;
				}

				average = ((p1 + p2 + p3 + p4) / 4) + (rand() % max / 5.0 - 1.0) * height;
				terrain[a][b + terrainStepSize / 2] = average;

				//SQUARE STEP 2

				count = 0;
				p1 = terrain[a][b];
				count++;
				p2 = terrain[a + terrainStepSize][b];
				count++;

				if ((b - terrainStepSize / 2) >= 0)
				{
					p3 = terrain[a + terrainStepSize / 2][b - terrainStepSize / 2];
					count++;
				}
				else
				{
					p3 = 0;
				}

				if ((b + terrainStepSize / 2) < MAP_SIZE)
				{
					p4 = terrain[a + terrainStepSize / 2][b + terrainStepSize / 2];
					count++;
				}
				else
				{
					p4 = 0;
				}

				average = ((p1 + p2 + p3 + p4) / 4) + (rand() % max / 5.0 - 1.0) * height;
				terrain[a + terrainStepSize / 2][b] = average;

				//SQUARE STEP 3

				count = 0;
				p1 = terrain[a + terrainStepSize][b];
				count++;
				p2 = terrain[a + terrainStepSize][b + terrainStepSize];
				count++;

				p3 = terrain[a + terrainStepSize / 2][b + terrainStepSize / 2];
				count++;

				if ((a + 3 * terrainStepSize / 2) < MAP_SIZE)
				{
					p4 = terrain[a + 3 * terrainStepSize / 2][b + terrainStepSize / 2];
					count++;
				}
				else
				{
					p4 = 0;
				}

				average = ((p1 + p2 + p3 + p4) / 4) + (rand() % max / 5.0 - 1.0) * height;
				terrain[a + terrainStepSize][b + terrainStepSize / 2] = average;

				//SQUARE STEP 4

				count = 0;
				p1 = terrain[a][b + terrainStepSize];
				count++;
				p2 = terrain[a + terrainStepSize][b + terrainStepSize];
				count++;

				p3 = terrain[a + terrainStepSize / 2][b + terrainStepSize / 2];
				count++;

				if ((b + 3 * terrainStepSize / 2) < MAP_SIZE)
				{
					p4 = terrain[a + terrainStepSize / 2][b + 3 * terrainStepSize / 2];
					count++;
				}
				else
				{
					p4 = 0;
				}

				average = ((p1 + p2 + p3 + p4) / 4) + (rand() % max / 5.0 - 1.0) * height;
				terrain[a + terrainStepSize / 2][b + terrainStepSize] = average;
			}
		}
		terrainStepSize = terrainStepSize / 2;
		height *= 0.5f;
		max--;

	}
	
	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			if (terrain[i][j] < -20)
			{
				terrain[i][j] = -20;
			}
		}
	}

	// Intialise vertex array
	int i = 0;
	int j = 0;

	//Generate texture Coords		FOR TERRAIN

	float terrainFTexS = float(MAP_SIZE) * 0.1f;
	float terrainFTexT = float(MAP_SIZE) * 0.1f;

	for (int z = 0; z < MAP_SIZE; z++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			
			terrainVertices[i].coords.x = (float)x;
			terrainVertices[i].coords.y = terrain[x][z];
			terrainVertices[i].coords.z = (float)z;
			terrainVertices[i].coords.w = 1.0;
			
			terrainVertices[i].normals.x = 0.0;
			terrainVertices[i].normals.y = 0.0;
			terrainVertices[i].normals.z = 0.0;

			float fScaleC = float(x) / float(MAP_SIZE - 1);
			float fScaleR = float(z) / float(MAP_SIZE - 1);

			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			terrainVertices[i].texcoords = vec2(terrainFTexS * fScaleC, terrainFTexT * fScaleR);
			
			i++;
		}
	}

	// Now build the index data      FOR TERRAIN
	i = 0;
	for (int z = 0; z < MAP_SIZE - 1; z++)
	{
		i = z * MAP_SIZE;
		for (int x = 0; x < MAP_SIZE * 2; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
		for (int x = 1; x < MAP_SIZE * 2 + 1; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
	}



	//Generate texture Coords		FOR WATER

	float waterFTexS = float(OCEAN_SIZE) * 0.1f;
	float waterFTexT = float(OCEAN_SIZE) * 0.1f;

	for (int z = 0; z < OCEAN_SIZE; z++)
	{
		for (int x = 0; x < OCEAN_SIZE; x++)
		{

			waterVertices[j].coords.x = (float)x;
			waterVertices[j].coords.y = water[x][z];
			waterVertices[j].coords.z = (float)z;
			waterVertices[j].coords.w = 1.0;

			waterVertices[j].normals.x = 0.0;
			waterVertices[j].normals.y = 0.0;
			waterVertices[j].normals.z = 0.0;

			float fScaleC = float(x) / float(OCEAN_SIZE - 1);
			float fScaleR = float(z) / float(OCEAN_SIZE - 1);

			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			waterVertices[j].texcoords = vec2(waterFTexS * fScaleC, waterFTexT * fScaleR);

			j++;
		}
	}

	// Now build the index data      FOR WATER
	j = 0;
	for (int z = 0; z < OCEAN_SIZE - 1; z++)
	{
		j = z * OCEAN_SIZE;
		for (int x = 0; x < OCEAN_SIZE * 2; x += 2)
		{
			waterIndexData[z][x] = j;
			j++;
		}
		for (int x = 1; x < OCEAN_SIZE * 2 + 1; x += 2)
		{
			waterIndexData[z][x] = j;
			j++;
		}
	}

	//Generate texture Coords		FOR CLOUDS
	j = 0;
	float cloudFTexS = float(CLOUD_SIZE) * 0.1f;
	float cloudFTexT = float(CLOUD_SIZE) * 0.1f;

	for (int z = 0; z < CLOUD_SIZE; z++)
	{
		for (int x = 0; x < CLOUD_SIZE; x++)
		{

			cloudVertices[j].coords.x = (float)x;
			cloudVertices[j].coords.y = cloud[x][z];
			cloudVertices[j].coords.z = (float)z;
			cloudVertices[j].coords.w = 1.0;

			cloudVertices[j].normals.x = 0.0;
			cloudVertices[j].normals.y = 1.0;
			cloudVertices[j].normals.z = 0.0;

			float fScaleC = float(x) / float(CLOUD_SIZE - 1);
			float fScaleR = float(z) / float(CLOUD_SIZE - 1);

			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			cloudVertices[j].texcoords = vec2(cloudFTexS * fScaleC, cloudFTexT * fScaleR);

			j++;
		}
	}

	// Now build the index data      FOR CLOUDS
	j = 0;
	for (int z = 0; z < CLOUD_SIZE - 1; z++)
	{
		j = z * CLOUD_SIZE;
		for (int x = 0; x < CLOUD_SIZE * 2; x += 2)
		{
			cloudIndexData[z][x] = j;
			j++;
		}
		for (int x = 1; x < CLOUD_SIZE * 2 + 1; x += 2)
		{
			cloudIndexData[z][x] = j;
			j++;
		}
	}

	// Normal Calculations			FOR TERRAIN
	int terrainIndex1, terrainIndex2, terrainIndex3;
	int terrainTotal;
	float terrainDotValue;

	glm::vec3 terrainPoint1, terrainPoint2, terrainPoint3, terrainTemporaryVec, terrainEdgeVec1, terrainEdgeVec2, terrainNormalVec, terrainUpVec;
	terrainUpVec.x = 0;
	terrainUpVec.y = 1;
	terrainUpVec.z = 0;

	for (int c = 0 ; c < MAP_SIZE - 1 ; c++)
	{
		for (int d = 0 ; d < MAP_SIZE * 2 - 2 ; d++)
		{
			terrainIndex1 = terrainIndexData[c][d];
			terrainIndex2 = terrainIndexData[c][d+1];
			terrainIndex3 = terrainIndexData[c][d+2];

			terrainPoint1.x = terrainVertices[terrainIndex1].coords.x;
			terrainPoint1.y = terrainVertices[terrainIndex1].coords.y;
			terrainPoint1.z = terrainVertices[terrainIndex1].coords.z;

			terrainPoint2.x = terrainVertices[terrainIndex2].coords.x;
			terrainPoint2.y = terrainVertices[terrainIndex2].coords.y;
			terrainPoint2.z = terrainVertices[terrainIndex2].coords.z;

			terrainPoint3.x = terrainVertices[terrainIndex3].coords.x;
			terrainPoint3.y = terrainVertices[terrainIndex3].coords.y;
			terrainPoint3.z = terrainVertices[terrainIndex3].coords.z;

			terrainEdgeVec1 = terrainPoint2 - terrainPoint1;
			terrainEdgeVec2 = terrainPoint3 - terrainPoint1;

			if (d%2 == 1)
			{
				terrainTemporaryVec = cross(terrainEdgeVec2, terrainEdgeVec1);
			}
			else
			{
				terrainTemporaryVec = cross(terrainEdgeVec1, terrainEdgeVec2);
			}

			terrainDotValue = dot(terrainTemporaryVec, terrainUpVec);
			if (terrainDotValue < 0.00000000001)
			{
				terrainNormalVec = -terrainTemporaryVec;
			}
			else
			{
				terrainNormalVec = terrainTemporaryVec;
			}

			terrainVertices[terrainIndex1].normals = terrainNormalVec + terrainVertices[terrainIndex1].normals;
			terrainVertices[terrainIndex2].normals = terrainNormalVec + terrainVertices[terrainIndex2].normals;
			terrainVertices[terrainIndex3].normals = terrainNormalVec + terrainVertices[terrainIndex3].normals;
		}
	}

	terrainTotal = MAP_SIZE * MAP_SIZE;
	for (int i = 0; i < (terrainTotal - 1); i++)
	{
		terrainTemporaryVec = terrainVertices[i].normals;
		terrainNormalVec = normalize(terrainTemporaryVec);
		terrainVertices[i].normals = terrainNormalVec;
	}


	// Normal Calculations			FOR WATER
	int waterIndex1, waterIndex2, waterIndex3;
	int waterTotal;
	float waterDotValue;

	glm::vec3 waterPoint1, waterPoint2, waterPoint3, waterTemporaryVec, waterEdgeVec1, waterEdgeVec2, waterNormalVec, waterUpVec;
	waterUpVec.x = 0;
	waterUpVec.y = 1;
	waterUpVec.z = 0;

	for (int c = 0; c < OCEAN_SIZE - 1; c++)
	{
		for (int d = 0; d < OCEAN_SIZE * 2 - 2; d++)
		{
			waterIndex1 = waterIndexData[c][d];
			waterIndex2 = waterIndexData[c][d + 1];
			waterIndex3 = waterIndexData[c][d + 2];

			waterPoint1.x = waterVertices[waterIndex1].coords.x;
			waterPoint1.y = waterVertices[waterIndex1].coords.y;
			waterPoint1.z = waterVertices[waterIndex1].coords.z;

			waterPoint2.x = waterVertices[waterIndex2].coords.x;
			waterPoint2.y = waterVertices[waterIndex2].coords.y;
			waterPoint2.z = waterVertices[waterIndex2].coords.z;

			waterPoint3.x = waterVertices[waterIndex3].coords.x;
			waterPoint3.y = waterVertices[waterIndex3].coords.y;
			waterPoint3.z = waterVertices[waterIndex3].coords.z;

			waterEdgeVec1 = waterPoint2 - waterPoint1;
			waterEdgeVec2 = waterPoint3 - waterPoint1;

			if (d % 2 == 1)
			{
				waterTemporaryVec = cross(waterEdgeVec2, waterEdgeVec1);
			}
			else
			{
				waterTemporaryVec = cross(waterEdgeVec1, waterEdgeVec2);
			}

			waterDotValue = dot(waterTemporaryVec, waterUpVec);
			if (waterDotValue < 0.00000000001)
			{
				waterNormalVec = -waterTemporaryVec;
			}
			else
			{
				waterNormalVec = waterTemporaryVec;
			}

			waterVertices[waterIndex1].normals = terrainNormalVec + waterVertices[waterIndex1].normals;
			waterVertices[waterIndex2].normals = terrainNormalVec + waterVertices[waterIndex2].normals;
			waterVertices[waterIndex3].normals = terrainNormalVec + waterVertices[waterIndex3].normals;
		}
	}

	waterTotal = OCEAN_SIZE * OCEAN_SIZE;
	for (int i = 0; i < (waterTotal - 1); i++)
	{
		waterTemporaryVec = waterVertices[i].normals;
		waterNormalVec = normalize(waterTemporaryVec);
		waterVertices[i].normals = waterNormalVec;
	}

	// Normal Calculations			FOR CLOUDS
	int cloudIndex1, cloudIndex2, cloudIndex3;
	int cloudTotal;
	float cloudDotValue;

	glm::vec3 cloudPoint1, cloudPoint2, cloudPoint3, cloudTemporaryVec, cloudEdgeVec1, cloudEdgeVec2, cloudNormalVec, cloudUpVec;
	cloudUpVec.x = 0;
	cloudUpVec.y = 1;
	cloudUpVec.z = 0;

	for (int c = 0; c < CLOUD_SIZE - 1; c++)
	{
		for (int d = 0; d < CLOUD_SIZE * 2 - 2; d++)
		{
			cloudIndex1 = cloudIndexData[c][d];
			cloudIndex2 = cloudIndexData[c][d + 1];
			cloudIndex3 = cloudIndexData[c][d + 2];

			cloudPoint1.x = cloudVertices[cloudIndex1].coords.x;
			cloudPoint1.y = cloudVertices[cloudIndex1].coords.y;
			cloudPoint1.z = cloudVertices[cloudIndex1].coords.z;

			cloudPoint2.x = cloudVertices[cloudIndex2].coords.x;
			cloudPoint2.y = cloudVertices[cloudIndex2].coords.y;
			cloudPoint2.z = cloudVertices[cloudIndex2].coords.z;

			cloudPoint3.x = cloudVertices[cloudIndex3].coords.x;
			cloudPoint3.y = cloudVertices[cloudIndex3].coords.y;
			cloudPoint3.z = cloudVertices[cloudIndex3].coords.z;

			cloudEdgeVec1 = cloudPoint2 - cloudPoint1;
			cloudEdgeVec2 = cloudPoint3 - cloudPoint1;

			if (d % 2 == 1)
			{
				cloudTemporaryVec = cross(cloudEdgeVec2, cloudEdgeVec1);
			}
			else
			{
				cloudTemporaryVec = cross(cloudEdgeVec1, cloudEdgeVec2);
			}

			cloudDotValue = dot(cloudTemporaryVec, cloudUpVec);
			if (cloudDotValue < 0.00000000001)
			{
				cloudNormalVec = -cloudTemporaryVec;
			}
			else
			{
				cloudNormalVec = cloudTemporaryVec;
			}

			cloudVertices[waterIndex1].normals = terrainNormalVec + cloudVertices[cloudIndex1].normals;
			cloudVertices[waterIndex2].normals = terrainNormalVec + cloudVertices[cloudIndex2].normals;
			cloudVertices[waterIndex3].normals = terrainNormalVec + cloudVertices[cloudIndex3].normals;
		}
	}

	cloudTotal = CLOUD_SIZE * CLOUD_SIZE;
	for (int i = 0; i < (cloudTotal - 1); i++)
	{
		cloudTemporaryVec = cloudVertices[i].normals;
		cloudNormalVec = normalize(cloudTemporaryVec);
		cloudVertices[i].normals = cloudNormalVec;
	}


	// order imageeeeeeeeee


	glClearColor(1.0, 1.0, 1.0, 0.0);

	skyboxNames.push_back("./Textures/skybox/mFRONT.bmp");
	skyboxNames.push_back("./Textures/skybox/mBACK.bmp");
	skyboxNames.push_back("./Textures/skybox/mUP.bmp");
	skyboxNames.push_back("./Textures/skybox/mDOWN.bmp");
	skyboxNames.push_back("./Textures/skybox/mRIGHT.bmp");
	skyboxNames.push_back("./Textures/skybox/mLEFT.bmp");
	
	

	skyboxMap = loadCubemap(skyboxNames);

	// Create shader program executable - read, compile and link shaders
	char* vertexShader = readTextFile("vertexShader.glsl");
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, (const char**)&vertexShader, NULL);
	glCompileShader(vertexShaderId);

	// Compiles the vertex shader
	glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
	glCompileShader(vertexShaderId);
	// Test for the vertex shader compilation errors
	std::cout << "VERTEX::" << std::endl;
	shaderCompileTest(vertexShaderId);

	char* fragmentShader = readTextFile("fragmentShader.glsl");
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, (const char**)&fragmentShader, NULL);
	glCompileShader(fragmentShaderId);

	// Compiles the vertex shader
	glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
	glCompileShader(fragmentShaderId);
	// Test for the vertex shader compilation errors
	std::cout << "FRAGMENT::" << std::endl;
	shaderCompileTest(fragmentShaderId);

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);
	///////////////////////////////////////

	// SETTING MATERIAL UNIFORM LOCATIONS FOR TERRAIN

	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.ambRefl"), 1,
		&terrainFandB.ambRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.difRefl"), 1,
		&terrainFandB.difRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.specRefl"), 1,
		&terrainFandB.specRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.emitCols"), 1,
		&terrainFandB.emitCols[0]);
	glUniform1f(glGetUniformLocation(programId, "terrainFandB.shininess"),
		terrainFandB.shininess);

	// SETTING MATERIAL UNIFORM LOCATIONS FOR WATER

	glUniform4fv(glGetUniformLocation(programId, "waterFandB.ambRefl"), 1,
		&waterFandB.ambRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "waterFandB.difRefl"), 1,
		&waterFandB.difRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "waterFandB.specRefl"), 1,
		&waterFandB.specRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "waterFandB.emitCols"), 1,
		&waterFandB.emitCols[0]);
	glUniform1f(glGetUniformLocation(programId, "waterFandB.shininess"),
		waterFandB.shininess);

	// SETTING MATERIAL UNIFORM LOCATIONS FOR CLOUD

	glUniform4fv(glGetUniformLocation(programId, "cloudFandB.ambRefl"), 1,
		&cloudFandB.ambRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "cloudFandB.difRefl"), 1,
		&cloudFandB.difRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "cloudFandB.specRefl"), 1,
		&cloudFandB.specRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "cloudFandB.emitCols"), 1,
		&cloudFandB.emitCols[0]);
	glUniform1f(glGetUniformLocation(programId, "cloudFandB.shininess"),
		cloudFandB.shininess);

	// SETTING AMBIENT UNIFORM LOCATION
	glUniform4fv(glGetUniformLocation(programId, "globAmb"), 1, &globAmb[0]);

	// SETTING DIRECTIONAL LIGHT UNIFORM LOCATION
	glUniform4fv(glGetUniformLocation(programId, "light0.ambCols"), 1,
		&light0.ambCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.difCols"), 1,
		&light0.difCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.specCols"), 1,
		&light0.specCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.coords"), 1,
		&light0.coords[0]);

	// SKYBOX SETUP

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 36, skyboxVertices, GL_STATIC_DRAW);

 	glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(9);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// TEXTURE							ADD ALL TEXTURES HERE

	image[0] = getbmp("Textures/grass.bmp");
	image[1] = getbmp("Textures/water.bmp");
	image[2] = getbmp("Textures/clouds.bmp");

	// CREATE TEXTURE ID

	glGenTextures(3, texture);

	// Bind the image FOR TERRAIN

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	grassTexLoc = glGetUniformLocation(programId, "grassTex");
	glUniform1i(grassTexLoc, 0);

	// Bind the image FOR WATER
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->sizeX, image[1]->sizeY, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	waterTexLoc = glGetUniformLocation(programId, "waterTex");
	glUniform1i(waterTexLoc, 1);

	// Bind the image FOR CLOUDS
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->sizeX, image[2]->sizeY, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	cloudTexLoc = glGetUniformLocation(programId, "cloudTex");
	glUniform1i(cloudTexLoc, 1);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Create vertex array object (VAO) and vertex buffer object (VBO) and associate data with vertex shader. FOR TERRAIN
	glGenVertexArrays(10, vao);
	glGenBuffers(10, buffer);
	glBindVertexArray(vao[TERRAIN]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[TERRAIN_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (GLvoid*)sizeof(terrainVertices[0].coords));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (GLvoid*)(sizeof(terrainVertices[0].coords) + sizeof(terrainVertices[0].normals)));
	glEnableVertexAttribArray(2);

	// Create vertex array object (VAO) and vertex buffer object (VBO) and associate data with vertex shader. FOR WATER
	glBindVertexArray(vao[WATER]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[WATER_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(waterVertices[0]), 0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(waterVertices[0]), (GLvoid*)sizeof(terrainVertices[0].coords));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(waterVertices[0]), (GLvoid*)(sizeof(waterVertices[0].coords) + sizeof(waterVertices[0].normals)));
	glEnableVertexAttribArray(5);

	glDisable(GL_CULL_FACE);
	// glCullFace(GL_BACK);

	// Create vertex array object (VAO) and vertex buffer object (VBO) and associate data with vertex shader. FOR CLOUDS
	glBindVertexArray(vao[CLOUDS]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[CLOUD_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertices), cloudVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(cloudVertices[0]), 0);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(cloudVertices[0]), (GLvoid*)sizeof(terrainVertices[0].coords));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(cloudVertices[0]), (GLvoid*)(sizeof(cloudVertices[0].coords) + sizeof(cloudVertices[0].normals)));
	glEnableVertexAttribArray(8);

	///////////////////////////////////////
	
	// Configure camera
	/*camera.SetFieldOfView(60.0f);
	camera.SetViewportAspectRatio((double)SCREEN_WIDTH / (double)SCREEN_HEIGHT);
	camera.SetNearAndFarPlanes(0.1f, 1000.0f);

	camera.MoveTo(glm::vef, 10.0f, 30.0f));
	camera.LookAt(glm::vec3(-15.0f, -5.0f, -50.0f));*/

	// Obtain projection matrix uniform location and set value.
	projMat = glm::perspective(radians(60.0), (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, 0.1, 1000.0);
	glUniformMatrix4fv(glGetUniformLocation(programId, "projMat"), 1, GL_FALSE, value_ptr(projMat));

	///////////////////////////////////////

	// Obtain modelview matrix uniform location and set value.
	// Move terrain into view - glm::translate replaces glTranslatef
	//modelViewMat = translate(modelViewMat, vec3(-2.5f, -2.5f, -10.0f)); // 5x5 grid
	// modelViewMat = translate(modelViewMat, vec3(-15.0f, -5.0f, -50.0f)); // 33x33 grid


	modelMatLoc = glGetUniformLocation(programId, "modelMat");
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, value_ptr(modelMat));

	viewMatLoc = glGetUniformLocation(programId, "ViewMat");
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, value_ptr(viewMat));

	///////////////////////////////////////
	static mat3 normalMat = mat3(1.0);

	normalMatLoc = glGetUniformLocation(programId, "normalMat");
	// Calculate and update normal matrix, after any changes to the view matrix
	normalMat = transpose(inverse(mat3(viewMat)));
	glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));

	
}

glm::vec3 cameraPos = glm::vec3(20.0f, 5.0f, 40.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
float Yaw = 90.0f;
float Pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
float fov = 45.0f;

void mouseMove(int x, int y)
{
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}
	float xoffset = x - lastX;
	float yoffset = lastY - y;
	lastX = x;
	lastY = y;

	float sensitivity = 0.3f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (Pitch > 89.0f) Pitch = 89.0f;
	if (Pitch < -89.0f) Pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(front);

	//forces mouse to stay in the window
	int win_w = glutGet(GLUT_WINDOW_WIDTH);
	int win_h = glutGet(GLUT_WINDOW_HEIGHT);
	if (x < 100 || x > win_w - 100)
	{
		lastX = win_w / 2;
		lastY = win_h / 2;
		glutWarpPointer(win_w / 2, win_h / 2);
	}
	else if (y < 100 || win_h - 100)
	{
		lastX = win_w / 2;
		lastY = win_h / 2;
		glutWarpPointer(win_w / 2, win_h / 2);
	}
}

float deltaTime = 0;

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(glGetUniformLocation(programId, "viewMat"), 1, GL_FALSE, value_ptr(viewMat));

	glBindVertexArray(vao[TERRAIN]);
// For each row - draw the triangle strip FOR TERRAIN
	glUniform1ui(glGetUniformLocation(programId, "object"), TERRAIN);
	glUniform1i(glGetUniformLocation(programId, "grassTex"), TERRAIN);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	for (int i = 0; i < MAP_SIZE - 1; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, terrainVerticesPerStrip, GL_UNSIGNED_INT, terrainIndexData[i]);
	}

	//glBindVertexArray(vao[WATER]);
	// For each row - draw the triangle strip FOR WATER
	glBindVertexArray(vao[WATER]);
	glUniform1f(glGetUniformLocation(programId, "time"), deltaTime);
	glUniform1ui(glGetUniformLocation(programId, "object"), WATER);
	glUniform1i(glGetUniformLocation(programId, "waterTex"), WATER);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	for (int i = 0; i < OCEAN_SIZE - 1; i++)
	{
		//glBindVertexArray(vao[WATER]);
		//glUniform1ui(glGetUniformLocation(programId, "objectType"), WATER);
	//	glUniform1f(waveLoc, waveIntensity);
		//glUniform1i(glGetUniformLocation(programId, "waterTex");
		
		glDrawElements(GL_TRIANGLE_STRIP, waterVerticesPerStrip, GL_UNSIGNED_INT, waterIndexData[i]);
	}

	glBindVertexArray(vao[CLOUDS]);
	// For each row - draw the triangle strip FOR CLOUDS
	
	glUniform1f(glGetUniformLocation(programId, "time"), deltaTime);
	glUniform1ui(glGetUniformLocation(programId, "object"), CLOUDS);
	glUniform1i(glGetUniformLocation(programId, "cloudTex"), CLOUDS);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	for (int i = 0; i < CLOUD_SIZE - 1; i++)
	{
		//glBindVertexArray(vao[WATER]);
		//glUniform1ui(glGetUniformLocation(programId, "objectType"), WATER);
		//	glUniform1f(waveLoc, waveIntensity);
			//glUniform1i(glGetUniformLocation(programId, "waterTex");
		
		glDrawElements(GL_TRIANGLE_STRIP, cloudVerticesPerStrip, GL_UNSIGNED_INT, cloudIndexData[i]);
	}


	//glDepthFunc(GL_LEQUAL);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	//
	//glUniform1ui(glGetUniformLocation(programId, "object"), 3);


	glBindVertexArray(skyboxVAO);
	//glActiveTexture(GL_TEXTURE0 + 3);
	glUniform1ui(glGetUniformLocation(programId, "object"), 3);
	glUniform1i(glGetUniformLocation(programId, "skybox"), 4);
	glUniform3f(glGetUniformLocation(programId, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMap);
	glDepthFunc(GL_LESS);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//glUniform1i(glGetUniformLocation(programId, "skybox"), 0);*/
	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	float cameraSpeed = 2.5f;
	switch (key)
	{
	case 'w':
		cameraPos += cameraFront * cameraSpeed;
		break;
	case 's':
		cameraPos -= cameraFront * cameraSpeed;
		break;
	case 'a':
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	case 'd':
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
	glutSwapBuffers();
}

static int oldTimeSinceStart;
static int newTimeSinceStart;


void updateGame()
{
	deltaTime += 0.001f;
	// cout << deltaTime << endl;
	if (deltaTime == 360) { deltaTime = 0; }
	/*oldTimeSinceStart = newTimeSinceStart;
	newTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = newTimeSinceStart - oldTimeSinceStart;
	if (deltaTime == 0) {
		Sleep(1);
		newTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		deltaTime = newTimeSinceStart - oldTimeSinceStart;
	}*/
	glutPostRedisplay();
}

// Main routine.
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);


	// Set the version of OpenGL (4.2)
	glutInitContextVersion(4, 2);
	// The core profile excludes all discarded features
	glutInitContextProfile(GLUT_CORE_PROFILE);
	// Forward compatibility excludes features marked for deprecation ensuring compatability with future versions
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("TerrainGeneration");

	// Set OpenGL to render in wireframe mode
	
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutIdleFunc(updateGame);
	glutKeyboardFunc(keyInput);
	glutPassiveMotionFunc(mouseMove);
	glEnable(GL_DEPTH_TEST);
	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	cout << "Use the W, A, S, and D keys to move the camera." << endl;
	cout << "MAKE SURE CAPS LOCK IS TURNED OFF." << endl;
	cout << "Move the mouse around to rotate the camera." << endl;

	glutMainLoop();
}
