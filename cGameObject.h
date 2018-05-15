#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>

// 1. Place physical properties into GO
// 2. Add a separate Physical Properties 
//    (i.e. a separate list of physical objects we are simulating)
//    -> Do we include GO into the Physical Property object?
//    -> Do we include the Physical Property into the GO?
//    -> Do we list them sep. in a 3rd list of "pairs"?

//class cGOPhysPair
//{
//	cPhysProps* pPhyOB;
//	cGameObject* pGO;
//}
//
//class cPhysProps
//{
//	glm::vec3 vel;			// Velocity
//	glm::vec3 accel;		// Acceleration'
//}

enum eTypeOfObject
{	// Ok people, here's the deal:
	SPHERE = 0,		// - it's a sphere
	PLANE = 1,		// - it's a plane
	BOX = 1,
	CAPSULE = 2,    // - it's a capsule
	AABB = 3,		// 3- it's an axis-aligned box
	UNKNOWN = 99	// I don't know
};

class cGameObject
{
public:
	cGameObject();		// constructor
	~cGameObject();		// destructor
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 orientation2;		// HACK (will elimiate this with)
	float scale;

	// **********************************
	// Add some physics things
	glm::vec3 vel;			// Velocity
	glm::vec3 accel;		// Acceleration
	bool bIsUpdatedInPhysics;		// 
	glm::vec3 length;
	glm::vec3 Height;
	glm::vec3 depth;
//	float mass;		INFINITY				// 
//	float inverseMass;	// = 0.0f	// 
	// Refers to the enum above
	eTypeOfObject typeOfObject;		// (really an int)
	float radius;
	
public:
	// **********************************

	glm::vec4 diffuseColour;	// 

	std::string meshName;		// mesh I'd like to draw
};

#endif
