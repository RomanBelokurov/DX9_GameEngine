		
////////////////////////////////////////////////////////////////////////////
//	physcis related classes for SPE
//		Copyright(c) spehome.com, All Rights Reserved.
//
//
///////////////////////////////////////////////////////////////////////////

#ifndef SPEPHYSICS_H
#define SPEPHYSICS_H

struct ISPEContact;
struct ISPEConstraint;
struct ISPERigidBody;


struct SPEJointDesc
{
	ISPERigidBody	*Body[2];
	SPEVector		BodyAnchor[2];
	SPEVector		WorldAnchor;
	float				MaxDistance;
	float				BreakForce;	
	bool				DetectCollision;
	bool				CreateByWorldAnchor;
	//float				Spring; //  no longer support
	//float				AngularFriction; // no longer support
	SPEJointDesc()
	{
		Default();
	}
	void Default()
	{
		Body[0] = Body[1] = 0;
		BodyAnchor[0] = SPEVector(0.0f, 0.0f, 0.0f);
		BodyAnchor[1] = SPEVector(0.0f, 0.0f, 0.0f);
		WorldAnchor = SPEVector(0.0f, 0.0f, 0.0f);
		MaxDistance = 0.0f;
		BreakForce = -1.0f;
		DetectCollision = true;
		CreateByWorldAnchor = true;
		//Spring = -1.0f;
		//AngularFriction = 0.1f;
	}

};

enum SPECONTACTTYPE
{
	SPE_CONTACT_COLLISION=0,
	SPE_CONTACT_JOINT,
	SPE_CONTACT_CUSTOM
};

struct SPEConstraintData
{
	SPEVector r[2];  // vector from center of mass to constraint point
	SPEVector n;  // normal
	float dv; // desired relative velocity in normal
	float u;  // friction coefficient in tangent space
	float low, high; // low and high bound of normal impulse

	SPEConstraintData()
	{
		Default();
	}
	void Default()
	{
		u = SPE_POSITIVE_INFINITE;
		low = SPE_NEGATIVE_INFINITE;
		high = SPE_POSITIVE_INFINITE;
	}
};

struct SPECastRayInfo
{
	ISPERigidBody *Body;
	SPEVector		Position;
	SPEVector		Normal;
	float				Distance;
};

enum SPEGROUPPHYSICSFLAG
{
	SPE_PHYSICS_NONE = 0x0,   // no collision detection and no solving
	SPE_PHYSICS_COLLISION = 0x1, // detect collision only but never solve
	SPE_PHYSICS_SOLVE = 0x2, // do not detect collision but solve constraints from Joint and Custom Contact
	SPE_PHYSICS_FULL = SPE_PHYSICS_COLLISION | SPE_PHYSICS_SOLVE, // both
};

enum SPEFLUIDMATERIAL
{
	SPE_WATER = 0,
	SPE_HWATER,
	SPE_SMOKE
};

#endif

