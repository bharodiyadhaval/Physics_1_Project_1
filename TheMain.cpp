#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>			// C++ cin, cout, etc.
//#include "linmath.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr


#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>		// "String stream"
#include <string>

#include <vector>		//  smart array, "array" in most languages

#include "Utilities.h"

#include "ModelUtilities.h"

#include "cMesh.h"

#include "cShaderManager.h" 

#include "cGameObject.h"

#include "cVAOMeshManager.h"

#include "Physics.h"	// Physics collision detection functions





int sobjects = 0, smodels = 0;
char* sfile = new char[15];
char* sname = new char[15];
float sscale, sx, sy, sz, sr, sg, sb, sorientpre, sorientpostx, sorientposty, sorientpostz;
// HACK plane collision detection and response
const float SURFACEOFGROUND = -5.0f;
const float RIGHTSIDEWALL = 15.0f;
const float LEFTSIDEWALL = -15.0f;


// This is the function "signature" for the 
//	function WAY at the bottom of the file.
void PhysicsStep( double deltaTime );


// Supposed to match the shader vertex layout...
//class cVertex
//{
//public:
//    float x, y, z;		// Position (vec2)	float x, y;	
//    float r, g, b;		// Colour (vec3)
//};


//cVertex vertices[3] =
//{
//	//  x       y    r      g    b
//    { -0.6f, -0.4f, 1.0f, 0.0f, 0.0f },	// Red
//    {  0.6f, -0.4f, 1.0f, 0.0f, 0.0f },	// Green
//    {   0.f,  0.6f, 1.0f, 0.0f, 0.0f }	// Blue
//};

// 453 vertices, so we'll make our array a little buffer

//cVertex vertices[4096];		// Stack based array (1Mbyte)
//cVertex* p_gVertices = 0;			// Heap based (to be an) array


//int g_numberOfVertices = 0;				// vertices
//int g_numberOfIndicies = 0;
//int g_numberOfTriangles = 0;			// faces

// This is a "c style" static allocated array
// Fixed in size, set at compile time, 
//  and in c, it's full of garbage values

//	static const int MAXNUMBEROFGAMEOBJECTS = 10;
//	cGameObject* g_GameObjects[MAXNUMBEROFGAMEOBJECTS];

// Remember to #include <vector>...
std::vector< cGameObject* >  g_vecGameObjects;


glm::vec3 g_cameraXYZ = glm::vec3( 0.0f, 0.0f, 10.0f );	// 5 units "down" z
glm::vec3 g_cameraTarget_XYZ = glm::vec3( 0.0f, 0.0f, 0.0f );

cVAOMeshManager* g_pVAOManager = 0;		// or NULL, or nullptr

//void ReadFileToToken( std::ifstream &file, std::string token )
//{
//	bool bKeepReading = true;
//	std::string garbage;
//	do
//	{
//		file >> garbage;		// Title_End??
//		if ( garbage == token )
//		{
//			return;
//		}
//	}while( bKeepReading );
//	return;
//}
// 
//
//
//
//bool LoadPlyObject( std::string filename )
//{
//	// c_str() changes a string to a "c style char* string"
//	std::ifstream plyFile( filename.c_str() );
//
//	if ( ! plyFile.is_open() )
//	{	// Didn't open file, so return
//		return false;
//	}
//	// File is open, let's read it
//
//	ReadFileToToken( plyFile, "vertex" );
////	int numVertices = 0;
//	plyFile >> g_numberOfVertices;		// numVertices
//
//	ReadFileToToken( plyFile, "face" );
////	int numIndices = 0;
//	plyFile >> g_numberOfTriangles;		// numIndices
//
//	ReadFileToToken( plyFile, "end_header" );
//
//	// Read vertices
//	for ( int index = 0; index < g_numberOfVertices; index++ )
//	{
//		//end_header
//		//-0.0312216 0.126304 0.00514924 0.850855 0.5 		
//		float x, y, z, confidence, intensity;
//
//		plyFile >> x;
//		plyFile >> y;
//		plyFile >> z;
//		plyFile >> confidence;
//		plyFile >> intensity;
//
//		vertices[index].x = x * 7.0f;
//		vertices[index].y = y * 7.0f;
//		vertices[index].r = 1.0f;
//		vertices[index].g = 1.0f;
//		vertices[index].b = 1.0f;
//	}
//
//	// Also add the triangles (aka "faces")
//	return true;
//}
//
//
//// C stlye 'string'
//static const char* /*string*/ vertex_shader_text =
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"
//"attribute vec2 vPos;\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
//"    color = vCol;"
//"    /* color.r = 0.0f; */ \n"
//"    /* color.g = 1.0f; */ \n"
//"    /* color.b = 0.0f; */ \n"
//"}\n";
//
//static const char* /*string*/ fragment_shader_text =
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_FragColor = vec4(color.r, color.g, color.b, 1.0);  \n"
//"}\n";

//cShaderManager	g_ShaderManager;		// Stack (no new)
cShaderManager*		g_pShaderManager;		// Heap, new (and delete)


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

	if ( key == GLFW_KEY_SPACE  )
	{
//		::g_GameObjects[1]->position.y += 0.01f;
		::g_vecGameObjects[1]->position.y += 0.01f;
	}

	const float CAMERASPEED = 0.1f;
	switch ( key )
	{
	case GLFW_KEY_A:		// Left
		g_cameraXYZ.x -= CAMERASPEED;
//		g_cameraTarget_XYZ.x -= CAMERASPEED;
		break;
	case GLFW_KEY_D:		// Right
		g_cameraXYZ.x += CAMERASPEED;
//		g_cameraTarget_XYZ.x += CAMERASPEED;
		break;
	case GLFW_KEY_W:		// Forward (along z)
		g_cameraXYZ.z += CAMERASPEED;
		break;
	case GLFW_KEY_S:		// Backwards (along z)
		g_cameraXYZ.z -= CAMERASPEED;
		break;
	case GLFW_KEY_Q:		// "Down" (along y axis)
		g_cameraXYZ.y -= CAMERASPEED;
		break;
	case GLFW_KEY_E:		// "Up" (along y axis)
		g_cameraXYZ.y += CAMERASPEED;
		break;
	case GLFW_KEY_UP:		// "Up" (along y axis)
		g_vecGameObjects[9]->vel.y = +fabs(g_vecGameObjects[9]->vel.y);
		break;
	case GLFW_KEY_DOWN:		// "Up" (along y axis)
		g_vecGameObjects[9]->vel.y = -fabs(g_vecGameObjects[9]->vel.y);
		break;
	case GLFW_KEY_LEFT:		// "Up" (along y axis)
		g_vecGameObjects[9]->vel.x = -fabs(g_vecGameObjects[9]->vel.x);
		break;
	case GLFW_KEY_RIGHT:		// "Up" (along y axis)
		g_vecGameObjects[9]->vel.x = +fabs(g_vecGameObjects[9]->vel.x);
		break;

	}
	return;
}


int main(void)
{

	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);


	// Print to the console...(if a console is there)
	//std::cout << "Hello" << std::endl;
	//int q = 8;
	//std::cout << "Type a number:";
	//std::cin >> q;
	//std::cout << "You typed " << q << ". Hazzah." << std::endl;

	int height = 480;	/* default */
	int width = 640;	// default
	std::string title = "OpenGL Rocks";

	std::ifstream infoFile("config.txt");
	if (!infoFile.is_open())
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so read it... 
		std::string a;

		infoFile >> a;	// "Game"	//std::cin >> a;
		infoFile >> a;	// "Config"
		infoFile >> a;	// "width"

		infoFile >> width;	// 1080

		infoFile >> a;	// "height"

		infoFile >> height;	// 768

		infoFile >> a;		// Title_Start

		std::stringstream ssTitle;		// Inside "sstream"
		bool bKeepReading = true;
		do
		{
			infoFile >> a;		// Title_End??
			if (a != "Title_End")
			{
				ssTitle << a << " ";
			}
			else
			{	// it IS the end! 
				bKeepReading = false;
				title = ssTitle.str();
			}
		} while (bKeepReading);


	}//if ( ! infoFile.is_open() )

	////////





	std::ifstream sceneobjload("scene_description.txt");
	if (!sceneobjload.is_open())
	{
		return false;
	}
	ReadFileToToken(sceneobjload, "models");
	sceneobjload >> smodels;

	for (int varj = 1; varj <= smodels; varj++)
	{

		ReadFileToToken(sceneobjload, "start" + std::to_string(varj));
		ReadFileToToken(sceneobjload, "name");
		sceneobjload >> sname;
		ReadFileToToken(sceneobjload, "objects");
		sceneobjload >> sobjects;


		for (int vark = 1; vark <= sobjects; vark++)
		{


			ReadFileToToken(sceneobjload, "object" + std::to_string(vark));
			ReadFileToToken(sceneobjload, "scale");
			sceneobjload >> sscale;
			ReadFileToToken(sceneobjload, "x");
			sceneobjload >> sx;
			ReadFileToToken(sceneobjload, "y");
			sceneobjload >> sy;
			ReadFileToToken(sceneobjload, "z");
			sceneobjload >> sz;

			ReadFileToToken(sceneobjload, "r");
			sceneobjload >> sr;
			ReadFileToToken(sceneobjload, "g");
			sceneobjload >> sg;
			ReadFileToToken(sceneobjload, "b");
			sceneobjload >> sb;

			ReadFileToToken(sceneobjload, "orientationpre");
			sceneobjload >> sorientpre;
			ReadFileToToken(sceneobjload, "orientationpostx");
			sceneobjload >> sorientpostx;
			ReadFileToToken(sceneobjload, "orientationposty");
			sceneobjload >> sorientposty;
			ReadFileToToken(sceneobjload, "orientationpostz");
			sceneobjload >> sorientpostz;

			cGameObject* pTempGO = new cGameObject();
			pTempGO->position.x = sx;
			pTempGO->position.y = sy;
			pTempGO->position.z = sz;
			pTempGO->orientation.z = glm::radians(sorientpre);	// Degrees
			pTempGO->orientation2.x = glm::radians(sorientpostx);
			pTempGO->orientation2.y = glm::radians(sorientposty);
			pTempGO->orientation2.z = glm::radians(sorientpostz);
			pTempGO->scale = sscale;
			pTempGO->diffuseColour = glm::vec4(sr, sg, sb, 1.0f);
			pTempGO->meshName = sname;
			if (varj <= 5)
			{
				pTempGO->bIsUpdatedInPhysics = false;
			}
			if (varj >= 6)
			{
				pTempGO->typeOfObject = eTypeOfObject::SPHERE;
				pTempGO->radius = sscale;
			}
			if (varj == 5 || varj == 4 || varj == 3 || varj == 2)
			{
				pTempGO->typeOfObject = eTypeOfObject::BOX;
			}
			//
			//	pTempGO->vel.y = 1.0f;
			//
			::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
		}

		//ReadFileToToken(sceneobjload, "end" + std::to_string(j));
	}





//	// Add a bunch more rabbits
//	const float SIZEOFWORLD = 6.0f;	
////	for ( int index = 3; index < MAXNUMBEROFGAMEOBJECTS; index++ )
//	for ( int index = 3; index < 100; index++ )
//	{
//		cGameObject* pTempGO = new cGameObject();
//		pTempGO->position.x = getRandInRange<float>(-SIZEOFWORLD, SIZEOFWORLD );
//		pTempGO->position.y = getRandInRange<float>(-SIZEOFWORLD, SIZEOFWORLD );
//		pTempGO->position.z = getRandInRange<float>(-SIZEOFWORLD, SIZEOFWORLD );
//		//::g_GameObjects[index]->scale = getRandInRange<float>( 7.0f, 15.0f );
//		// Pick a random colour for this bunny
//		pTempGO->diffuseColour.r = getRandInRange<float>(0.0f, 1.0f );
//		pTempGO->diffuseColour.g = getRandInRange<float>(0.0f, 1.0f );
//		pTempGO->diffuseColour.b = getRandInRange<float>(0.0f, 1.0f );
//		pTempGO->meshName = "bunny";
//		::g_vecGameObjects.push_back( pTempGO );
//	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// C++ string
	// C no strings. Sorry. char    char name[7] = "Michael\0";
    window = glfwCreateWindow( width, height, 
							   title.c_str(), 
							   NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

	std::cout << glGetString(GL_VENDOR) << " " 
		<< glGetString(GL_RENDERER) << ", " 
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;



	::g_pShaderManager = new cShaderManager();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";	
	fragShader.fileName = "simpleFrag.glsl"; 

	::g_pShaderManager->setBasePath( "assets//shaders//" );


	if ( ! ::g_pShaderManager->createProgramFromFile(
		        "mySexyShader", vertShader, fragShader ) )
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;	
//		exit(
	}
	std::cout << "The shaders comipled and linked OK" << std::endl;


	// Load models
	::g_pVAOManager = new cVAOMeshManager();

	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
	/////////////////////////////////
	std::ifstream sceneload("scene_description.txt");
	if (!sceneload.is_open())
	{
		return false;
	}

	ReadFileToToken(sceneload, "models");
	sceneload >> smodels;
	for (int j = 1; j <= smodels; j++)
	{

		ReadFileToToken(sceneload, "start" + std::to_string(j));
		ReadFileToToken(sceneload, "file");
		sceneload >> sfile;
		ReadFileToToken(sceneload, "name");
		sceneload >> sname;

		//ReadFileToToken(sceneload, "end"+std::to_string(j));
		cMesh testMesh;
		testMesh.name = sname;
		if (!LoadPlyFileIntoMesh(sfile, testMesh))
		{
			std::cout << "Didn't load model" << std::endl;
			// do something??
		}
		if (!::g_pVAOManager->loadMeshIntoVAO(testMesh, sexyShaderID))
		{
			std::cout << "Could not load mesh into VAO" << std::endl;
		}
	}

		






		//{
		//	cMesh testMesh;
		//	testMesh.name = "PlaneXZ";
		//	if (!LoadPlyFileIntoMesh("Flat_XZ_Plane_xyz.ply", testMesh))
		//	{
		//		std::cout << "Didn't load model" << std::endl;
		//		// do something??
		//	}
		//	if (!::g_pVAOManager->loadMeshIntoVAO(testMesh, sexyShaderID))
		//	{
		//		std::cout << "Could not load mesh into VAO" << std::endl;
		//	}
		//}	// ENDOF: load models
		//{
		//	cMesh testMesh;
		//	testMesh.name = "box";
		//	if (!LoadPlyFileIntoMesh("box.ply", testMesh))
		//	{
		//		std::cout << "Didn't load model" << std::endl;
		//		// do something??
		//	}
		//	if (!::g_pVAOManager->loadMeshIntoVAO(testMesh, sexyShaderID))
		//	{
		//		std::cout << "Could not load mesh into VAO" << std::endl;
		//	}
		//}	// ENDOF: load models
		//{
		//	cMesh testMesh;
		//	testMesh.name = "box2";
		//	if (!LoadPlyFileIntoMesh("box2.ply", testMesh))
		//	{
		//		std::cout << "Didn't load model" << std::endl;
		//		// do something??
		//	}
		//	if (!::g_pVAOManager->loadMeshIntoVAO(testMesh, sexyShaderID))
		//	{
		//		std::cout << "Could not load mesh into VAO" << std::endl;
		//	}
		//}	// ENDOF: load models
		//{
		//	cMesh testMesh;
		//	testMesh.name = "box3";
		//	if (!LoadPlyFileIntoMesh("box3.ply", testMesh))
		//	{
		//		std::cout << "Didn't load model" << std::endl;
		//		// do something??
		//	}
		//	if (!::g_pVAOManager->loadMeshIntoVAO(testMesh, sexyShaderID))
		//	{
		//		std::cout << "Could not load mesh into VAO" << std::endl;
		//	}
		//}	// ENDOF: load models
		//{
		//	cMesh testMesh;
		//	testMesh.name = "box4";
		//	if (!LoadPlyFileIntoMesh("box4.ply", testMesh))
		//	{
		//		std::cout << "Didn't load model" << std::endl;
		//		// do something??
		//	}
		//	if (!::g_pVAOManager->loadMeshIntoVAO(testMesh, sexyShaderID))
		//	{
		//		std::cout << "Could not load mesh into VAO" << std::endl;
		//	}
		//}	// ENDOF: load models
		//{
		//	cMesh testMesh;
		//	testMesh.name = "SphereRadius1";
		//	if (!LoadPlyFileIntoMesh("Sphereply_xyz.ply", testMesh))
		//	{
		//		std::cout << "Didn't load model" << std::endl;
		//		// do something??
		//	}
		//	if (!::g_pVAOManager->loadMeshIntoVAO(testMesh, sexyShaderID))
		//	{
		//		std::cout << "Could not load mesh into VAO" << std::endl;
		//	}
		//}	// ENDOF: load models
		////vertex_shader = glCreateShader(GL_VERTEX_SHADER);


	




	//fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

 //   glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
 //   glCompileShader(vertex_shader);

 //   glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
 //   glCompileShader(fragment_shader);

	//program = glCreateProgram();

	//glAttachShader(program, vertex_shader);
 //   glAttachShader(program, fragment_shader);

	//glLinkProgram(program);

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

	mvp_location = glGetUniformLocation(currentProgID, "MVP");		// program, "MVP");



	glEnable( GL_DEPTH );

	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();

	// Main game or application loop
	while ( ! glfwWindowShouldClose(window) )
    {
        float ratio;
        int width, height;
        glm::mat4x4 m, p, mvp;			//  mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT );



		// "Draw scene" loop
//		for ( int index = 0; index != MAXNUMBEROFGAMEOBJECTS; index++ )
		 std::vector< cGameObject* > g_vecGameObjects;

		unsigned int sizeOfVector = ::g_vecGameObjects.size();	//*****//
		for ( int index = 0; index != sizeOfVector; index++ )
		{
			// Is there a game object? 
			if ( ::g_vecGameObjects[index] == 0 )	//if ( ::g_GameObjects[index] == 0 )
			{	// Nothing to draw
				continue;		// Skip all for loop code and go to next
			}

			// Was near the draw call, but we need the mesh name
			std::string meshToDraw = ::g_vecGameObjects[index]->meshName;		//::g_GameObjects[index]->meshName;

			sVAOInfo VAODrawInfo;
			if ( ::g_pVAOManager->lookupVAOFromName( meshToDraw, VAODrawInfo ) == false )
			{	// Didn't find mesh
				continue;
			}



			// There IS something to draw

			m = glm::mat4x4(1.0f);	//		mat4x4_identity(m);

	
			glm::mat4 matRreRotZ = glm::mat4x4(1.0f);
			matRreRotZ = glm::rotate( matRreRotZ, ::g_vecGameObjects[index]->orientation.z, 
								     glm::vec3(0.0f, 0.0f, 1.0f) );
			m = m * matRreRotZ;

			glm::mat4 trans = glm::mat4x4(1.0f);
			trans = glm::translate( trans, 
								    ::g_vecGameObjects[index]->position );
			m = m * trans; 

			glm::mat4 matPostRotZ = glm::mat4x4(1.0f);
			matPostRotZ = glm::rotate( matPostRotZ, ::g_vecGameObjects[index]->orientation2.z, 
								     glm::vec3(0.0f, 0.0f, 1.0f) );
			m = m * matPostRotZ;

//			::g_vecGameObjects[index]->orientation2.y += 0.01f;

			glm::mat4 matPostRotY = glm::mat4x4(1.0f);
			matPostRotY = glm::rotate( matPostRotY, ::g_vecGameObjects[index]->orientation2.y, 
								     glm::vec3(0.0f, 1.0f, 0.0f) );
			m = m * matPostRotY;


			glm::mat4 matPostRotX = glm::mat4x4(1.0f);
			matPostRotX = glm::rotate( matPostRotX, ::g_vecGameObjects[index]->orientation2.x, 
								     glm::vec3(1.0f, 0.0f, 0.0f) );
			m = m * matPostRotX;
		
			float finalScale = ::g_vecGameObjects[index]->scale;

			glm::mat4 matScale = glm::mat4x4(1.0f);
			matScale = glm::scale( matScale, 
								   glm::vec3( finalScale,
								              finalScale,
								              finalScale ) );
			m = m * matScale;


			//mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
//			p = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f );
			p = glm::perspective( 0.6f,			// FOV
								  ratio,		// Aspect ratio
								  0.1f,			// Near (as big as possible)
								  1000.0f );	// Far (as small as possible)

			// View or "camera" matrix
			glm::mat4 v = glm::mat4(1.0f);	// identity

			//glm::vec3 cameraXYZ = glm::vec3( 0.0f, 0.0f, 5.0f );	// 5 units "down" z
			v = glm::lookAt( g_cameraXYZ,						// "eye" or "camera" position
							 g_cameraTarget_XYZ,		// "At" or "target" 
							 glm::vec3( 0.0f, 1.0f, 0.0f ) );	// "up" vector

			//mat4x4_mul(mvp, p, m);
//			mvp = p * m;
//			mvp = m * v * p;			// <---- this way
			mvp = p * v * m;			// This way (sort of backwards)

			GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
			GLint diffuseColour_loc = glGetUniformLocation( shaderID, "diffuseColour" );

			glUniform4f( diffuseColour_loc, 
						::g_vecGameObjects[index]->diffuseColour.r, 
						::g_vecGameObjects[index]->diffuseColour.g, 
						::g_vecGameObjects[index]->diffuseColour.b, 
						::g_vecGameObjects[index]->diffuseColour.a );


	//        glUseProgram(program);
			::g_pShaderManager->useShaderProgram( "mySexyShader" );

			//glUniformMatrix4fv(mvp_location, 1, GL_FALSE, 
			//                 (const GLfloat*) mvp);
			glUniformMatrix4fv(mvp_location, 1, GL_FALSE, 
							   (const GLfloat*) glm::value_ptr(mvp) );

	//		glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		//	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );	// Default

		
				glBindVertexArray( VAODrawInfo.VAO_ID );

				glDrawElements( GL_TRIANGLES, 
								VAODrawInfo.numberOfIndices,		// testMesh.numberOfTriangles * 3,	// How many vertex indices
								GL_UNSIGNED_INT,					// 32 bit int 
								0 );

				glBindVertexArray( 0 );


		}//for ( int index = 0...


		std::stringstream ssTitle;
		ssTitle << "Camera (xyz): " 
			<< g_cameraXYZ.x << ", " 
			<< g_cameraXYZ.y << ", " 
			<< g_cameraXYZ.z;
		glfwSetWindowTitle( window, ssTitle.str().c_str() );

        glfwSwapBuffers(window);
        glfwPollEvents();


		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime =  curTime - lastTimeStep;

	PhysicsStep( deltaTime );

		lastTimeStep = curTime;



    }// while ( ! glfwWindowShouldClose(window) )


    glfwDestroyWindow(window);
    glfwTerminate();

	// 
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;

//    exit(EXIT_SUCCESS);
	return 0;
}

// Update the world 1 "step" in time
void PhysicsStep( double deltaTime )
{
	
	for ( int index = 0; index != ::g_vecGameObjects.size(); index++ )
	{
		cGameObject* pCurGO = ::g_vecGameObjects[index];
		pCurGO->diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		if (index == 9)
		{
			pCurGO->diffuseColour = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		}
	}//for ( int index


	const glm::vec3 GRAVITY = glm::vec3(-0.20f, -1.5f, 0.0f);

	// Identical to the 'render' (drawing) loop
	for ( int index = 0; index != ::g_vecGameObjects.size(); index++ )
	{
		
		cGameObject* pCurGO = ::g_vecGameObjects[index];

		// Is this object to be updated?
		if ( ! pCurGO->bIsUpdatedInPhysics )
		{	// DON'T update this
			continue;		// Skip everything else in the for
		}

		// Explicit Euler  (RK4) 
		// New position is based on velocity over time
		glm::vec3 deltaPosition = (float)deltaTime * pCurGO->vel;
		pCurGO->position += deltaPosition;

		// New velocity is based on acceleration over time
		glm::vec3 deltaVelocity =  ( (float)deltaTime * pCurGO->accel )
			                     + ( (float)deltaTime * GRAVITY );

		pCurGO->vel += deltaVelocity;

		// HACK: Collision step
		// Assume the "ground" is -2.0f from the origin in the y
		switch ( pCurGO->typeOfObject )
		{
		case  eTypeOfObject::SPHERE:	
			// Comare this to EVERY OTHER object in the scene
			for ( int indexEO = 0; indexEO != ::g_vecGameObjects.size(); indexEO++ )
			{
				// Don't test for myself
				if ( index == indexEO )		
					continue;	// It's me!! 

				cGameObject* pOtherObject = ::g_vecGameObjects[indexEO];
				// Is Another object
				switch ( pOtherObject->typeOfObject )
				{
				case eTypeOfObject::SPHERE:
					// 

		
					if ( PenetrationTestSphereSphere( pCurGO, pOtherObject ) )
					{
						
						//std::cout << "Collision!" << std::endl;
						pCurGO->diffuseColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

						pOtherObject->diffuseColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
					
								//::g_vecGameObjects[3]->diffuseColour = glm::vec4(0.1f, 1.0f, 0.0f, 1.0f);

						if (glm::distance(pCurGO->position, pOtherObject->position)<2*pCurGO->radius)
						{
							pCurGO->vel = -((pCurGO->vel));
							pOtherObject->vel = -((pOtherObject->vel));
						}
						
					}
					break;

				}//switch ( pOtherObject->typeOfObject )

			}//for ( int index
	
			// Sphere-Plane detection
			for (int i = 7; i <= 17; i++)
			{
				if (::g_vecGameObjects[i]->position.x - ::g_vecGameObjects[i]->radius < -6.8f)
				{
					//Far too left
					::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
					::g_vecGameObjects[i]->vel.x = +(fabs(::g_vecGameObjects[i]->vel.x));
				}
				if (::g_vecGameObjects[i]->position.x - ::g_vecGameObjects[i]->radius >= -6.8 &&::g_vecGameObjects[i]->position.x + ::g_vecGameObjects[i]->radius < -2.5)
				{
					//Stair1
					if ((::g_vecGameObjects[i]->position.y - ::g_vecGameObjects[i]->radius) <= -2.1f)
					{
						::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
						::g_vecGameObjects[3]->diffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
						::g_vecGameObjects[i]->vel.y = +(fabs(::g_vecGameObjects[i]->vel.y));
					}
					
					if ((::g_vecGameObjects[i]->position.y - ::g_vecGameObjects[i]->radius) > - 2.1f && (::g_vecGameObjects[i]->position.y + ::g_vecGameObjects[i]->radius) <= 2.1f)
					{
						if ((::g_vecGameObjects[i]->position.x + ::g_vecGameObjects[i]->radius) >= -3.1)
						{
							::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
							::g_vecGameObjects[4]->diffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
							::g_vecGameObjects[i]->vel.x = -(fabs(::g_vecGameObjects[i]->vel.x));
						}
						
					}
					
					//
				}
				else if (::g_vecGameObjects[i]->position.x - ::g_vecGameObjects[i]->radius >= -4.2 &&::g_vecGameObjects[i]->position.x + ::g_vecGameObjects[i]->radius < 1)
				{
					//Stair2
					if ((::g_vecGameObjects[i]->position.y - ::g_vecGameObjects[i]->radius) <= 1.35f)
					{
						::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
						::g_vecGameObjects[4]->diffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
						::g_vecGameObjects[i]->vel.y = +(fabs(::g_vecGameObjects[i]->vel.y));
					}

					if ((::g_vecGameObjects[i]->position.y - ::g_vecGameObjects[i]->radius) > 1.35f && (::g_vecGameObjects[i]->position.y + ::g_vecGameObjects[i]->radius) <=5.35f)
					{
						if ((::g_vecGameObjects[i]->position.x + ::g_vecGameObjects[i]->radius) >= 0.3)
						{
							::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
							::g_vecGameObjects[5]->diffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
							::g_vecGameObjects[i]->vel.x = -(fabs(::g_vecGameObjects[i]->vel.x));
						}

					}
					//
				}
				else if (::g_vecGameObjects[i]->position.x - ::g_vecGameObjects[i]->radius >= -0.50 &&::g_vecGameObjects[i]->position.x + ::g_vecGameObjects[i]->radius < 5)
				{
					//Stair3
					if ((::g_vecGameObjects[i]->position.y - ::g_vecGameObjects[i]->radius) <= 5.0f)
					{
						::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
						//	::g_vecGameObjects[3]->diffuseColour = glm::vec4(0.1f, 1.0f, 0.0f, 1.0f);
						::g_vecGameObjects[5]->diffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
						::g_vecGameObjects[i]->vel.y = +(fabs(::g_vecGameObjects[i]->vel.y));
					}
					

					if ((::g_vecGameObjects[i]->position.y - ::g_vecGameObjects[i]->radius) > 5.0f && (::g_vecGameObjects[i]->position.y + ::g_vecGameObjects[i]->radius) <= 9.2f)
					{
						if ((::g_vecGameObjects[i]->position.x + ::g_vecGameObjects[i]->radius) >= 4.2)
						{
							::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
							::g_vecGameObjects[6]->diffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
							::g_vecGameObjects[i]->vel.x = -(fabs(::g_vecGameObjects[i]->vel.x));
						}

					}
					//
				}
				else if (::g_vecGameObjects[i]->position.x - ::g_vecGameObjects[i]->radius >= 4 &&::g_vecGameObjects[i]->position.x + ::g_vecGameObjects[i]->radius < 9.0f)
				{
					//Stair4
					if ((::g_vecGameObjects[i]->position.y - ::g_vecGameObjects[i]->radius) <= 8.5f)
					{
						::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
						//	::g_vecGameObjects[3]->diffuseColour = glm::vec4(0.1f, 1.0f, 0.0f, 1.0f);
						::g_vecGameObjects[6]->diffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
						::g_vecGameObjects[i]->vel.y = +(fabs(::g_vecGameObjects[i]->vel.y));
					}
					
					//
				}
				if (::g_vecGameObjects[i]->position.x + ::g_vecGameObjects[i]->radius > 7.7f)
				{
					//far too right
					::g_vecGameObjects[i]->diffuseColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
					::g_vecGameObjects[i]->vel.x = -(fabs(::g_vecGameObjects[i]->vel.x));
				}
			}
			break;
		};


	}//for ( int index...

	return;
}


