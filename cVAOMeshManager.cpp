#include <glad/glad.h>		// For all the OpenGL calls
#include <GLFW/glfw3.h>		// For all the OpenGL calls

#include "cVAOMeshManager.h"
#include "cMesh.h"	// NOTE: This is in the CPP file, NOT the .h file

// The vertex layout as defined by the shader
// Where's the Best Place to put this?? 
// (good question)
class cVertex
{
public:
    float x, y, z;		// Position (vec2)	float x, y;	
    float r, g, b;		// Colour (vec3)
};

cVAOMeshManager::cVAOMeshManager()
{

	return;
}

cVAOMeshManager::~cVAOMeshManager()
{

	return;
}

bool cVAOMeshManager::loadMeshIntoVAO( cMesh &theMesh, int shaderID )
{
// ******************************************************************************
//__      __       _              ____         __  __          
//\ \    / /      | |            |  _ \       / _|/ _|         
// \ \  / /__ _ __| |_ _____  __ | |_) |_   _| |_| |_ ___ _ __ 
//  \ \/ / _ \ '__| __/ _ \ \/ / |  _ <| | | |  _|  _/ _ \ '__|
//   \  /  __/ |  | ||  __/>  <  | |_) | |_| | | | ||  __/ |   
//    \/ \___|_|   \__\___/_/\_\ |____/ \__,_|_| |_| \___|_|   
//                                                             
// ******************************************************************************

	sVAOInfo theVAOInfo;

	// Create a Vertex Array Object (VAO)
	glGenVertexArrays( 1, &(theVAOInfo.VAO_ID) );
	glBindVertexArray( theVAOInfo.VAO_ID );


    glGenBuffers(1, &(theVAOInfo.vertex_buffer_ID) );
    glBindBuffer(GL_ARRAY_BUFFER, theVAOInfo.vertex_buffer_ID);

	// Allocate the global vertex array
	cVertex* pVertices = new cVertex[theMesh.numberOfVertices];

	for ( int index = 0; index < theMesh.numberOfVertices; index++ )
	{
		pVertices[index].x = theMesh.pVertices[index].x;
		pVertices[index].y = theMesh.pVertices[index].y;
		pVertices[index].z = theMesh.pVertices[index].z;	

		pVertices[index].r = theMesh.pVertices[index].r;
		pVertices[index].g = theMesh.pVertices[index].g;
		pVertices[index].b = theMesh.pVertices[index].b;
	}

	// Copy the local vertex array into the GPUs memory
	int sizeOfGlobalVertexArrayInBytes = sizeof(cVertex) * theMesh.numberOfVertices;
    glBufferData(GL_ARRAY_BUFFER, 
				 sizeOfGlobalVertexArrayInBytes,		// sizeof(vertices), 
				 pVertices, 
				 GL_STATIC_DRAW);

	// Get rid of local vertex array
	delete [] pVertices;
// ******************************************************************************
// ******************************************************************************

	glGenBuffers( 1, &(theVAOInfo.index_buffer_ID) );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, theVAOInfo.index_buffer_ID ); // It's now an Index Buffer

	// Make a temporary array that matches what will be on the GPU...
	// (GPU expects a 1D array of integers)

	// Number of indices = number of tris x 3 (3 indices per triangle)
	int numberOfIndices = theMesh.numberOfTriangles * 3;

	//unsigned int indexArray[ 10000000 ];		// CAN'T DO THIS, unfortunately
	unsigned int* indexArray = new unsigned int[numberOfIndices];
	
	// Copy the triangle data into this 1D array 
	int triIndex = 0;		// Index into the triangle array (from mesh)
	int indexIndex = 0;		// Index into the "vertex index" array (1D)
	for (  ;  triIndex < theMesh.numberOfTriangles; triIndex++, indexIndex += 3 )
	{
		indexArray[ indexIndex + 0 ] = theMesh.pTriangles[triIndex].vertex_ID_0;
		indexArray[ indexIndex + 1 ] = theMesh.pTriangles[triIndex].vertex_ID_1;
		indexArray[ indexIndex + 2 ] = theMesh.pTriangles[triIndex].vertex_ID_2;
	}
	// note number of indices is number of triangles x 3
	int sizeOfIndexArrayInBytes = sizeof(unsigned int) * numberOfIndices;

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,		// "index buffer" or "vertex index buffer"
				 sizeOfIndexArrayInBytes,		
				 indexArray, 
				 GL_STATIC_DRAW);

	// Don't need local array anymore (like the on in CPU RAM)
	delete [] indexArray;

	// At this point, the vertex and index buffers are stored on GPU

	// Now set up the vertex layout (for this shader)

	GLuint vpos_location = glGetAttribLocation(shaderID, "vPos");		// program, "vPos");	// 6
    GLuint vcol_location = glGetAttribLocation(shaderID, "vCol");		// program, "vCol");	// 13

//	vec3 vPos,		x = 0th location in this class
//	vec3 vCol       r = 3rd location in this class
    glEnableVertexAttribArray(vpos_location);
	const int offsetOf_x_into_cVertex = 0;	// X is 0th location in cVertex
    glVertexAttribPointer(vpos_location, 
						  3,				// now vec3, not vec2   
						  GL_FLOAT, 
						  GL_FALSE,
                          sizeof(float) * 6,	// cVertex is 6 floats in size
					//	  (void*) (sizeof(float) * offsetOf_x_into_cVertex) );		
						  (void*) offsetof( cVertex, x ) );


    glEnableVertexAttribArray(vcol_location);
	const int offsetOf_r_into_cVertex = 3;	// "r" is 3rd float into cVertex
    glVertexAttribPointer(vcol_location, 
						  3, 
						  GL_FLOAT, 
						  GL_FALSE,
                          sizeof(float) * 6,		// (  sizeof(cVertex)  );
				      //  (void*) (sizeof(float) * offsetOf_r_into_cVertex));
						  (void*) offsetof( cVertex, r ) );

	// Copy the information into the VAOInfo structure
	theVAOInfo.numberOfIndices = theMesh.numberOfTriangles * 3;
	theVAOInfo.numberOfTriangles = theMesh.numberOfTriangles;
	theVAOInfo.numberOfVertices = theMesh.numberOfVertices;
	theVAOInfo.friendlyName = theMesh.name;
	theVAOInfo.shaderID = shaderID;


	theMesh.CalculateExtents();
	theVAOInfo.scaleForUnitBBox = theMesh.scaleForUnitBBox;

	// Store the VAO info by mesh name
	this->m_mapNameToVAO[ theMesh.name ] = theVAOInfo;

	//std::string myArray[10];
	//myArray[3] = "Michael"

	// CRITICAL 
	// Unbind the VAO first!!!!
	glBindVertexArray( 0 );	// 

	// Unbind (release) everything
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vpos_location);

	return true;
}

bool cVAOMeshManager::lookupVAOFromName( std::string name, sVAOInfo &theVAOInfo )
{
	// look up in map for the name of the mesh we want to draw

	 //	std::map< std::string, sVAOInfo > m_mapNameToVAO;
	// "Interator" is a class that can access inside a container
	std::map< std::string, sVAOInfo >::iterator itVAO = this->m_mapNameToVAO.find( name );

	// Did I find something?
	if ( itVAO == this->m_mapNameToVAO.end() )
	{	// ON NO! we DIDN'T!
		return false;
	}
	// DID find what we were looking for, so 
	//	ISN'T pointing to the "end()" built-in iterator
	theVAOInfo = itVAO->second;		// Because the "second" thing is the sVAO
}
