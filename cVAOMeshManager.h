#ifndef _cVAOMeshManager_HG_
#define _cVAOMeshManager_HG_

//#include <GLFW\glfw3.h>	// Give us opengl.h 
#include <string>
//#include "cMesh.h"
// Instead of including, which means possibly including 
//	stuff we don't need or want to know about, we 
//  'forward declare'
class cMesh;		// Forward declare

#include <map>		// "dictonary" or PHP array

struct sVAOInfo
{
	sVAOInfo() : VAO_ID(0), shaderID(-1), 
		         vertex_buffer_ID(0), index_buffer_ID(0),
		         numberOfVertices(0), numberOfTriangles(0),
				 numberOfIndices(0), 
				 scaleForUnitBBox(1.0f)
	{ }
	std::string friendlyName;
	// OpenGL rendering specific stuff we need...
	unsigned int VAO_ID;	// **** The thing we REALLY want

	// We might need this information to draw (or "render")
	unsigned int numberOfVertices;
	unsigned int numberOfTriangles;
	unsigned int numberOfIndices;		/****/


	int shaderID;			// Perhaps?
	// Spoiler alert: we don't need these, actually...
	unsigned int vertex_buffer_ID;		// GLuint
	unsigned int index_buffer_ID;

	float scaleForUnitBBox;
};

// This will handle: 
// - taking a cMesh and loading into the various buffers
// - take a mesh "name" and returning the desired VAO info 
//   (for rendering)
class cVAOMeshManager
{
public:
	cVAOMeshManager();
	~cVAOMeshManager();
	// Take name from mesh for lookup (for rendering)
	bool loadMeshIntoVAO( cMesh &theMesh, int shaderID );

	// During rendering (aka 'drawing'), get the info we need.
	bool lookupVAOFromName( std::string name, sVAOInfo &theVAOInfo );

	std::map<int, std::string> mapNumberToName;

private:
	// Loop up from name to sVAOInfo
	// 1st is what I'm indexing by (i.e. type)
	// 2nd is what I'm actually storing (the type)
	std::map< std::string, sVAOInfo > m_mapNameToVAO;

//	sVAOInfo theVAOS[10];
};


#endif
