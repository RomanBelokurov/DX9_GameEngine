#ifndef SPE_H
#define SPE_H
#define SPE_VERSION 3.20

#include "SPELib.h"
#include "SPEArray.h"
#include "SPEVector_Matrix.h"
#include "SPEGeometry.h"
#include "SPEPhysics.h"
#include "SPEMesh.h"

//////////////////////////////////////////
//
//		Simple Physics Engine (SPE)
//		Copyright(c) Phyar Lab(spehome.com), All Rights Reserved.
//
//////////////////////////////////////////

struct ISPEShape
{
	virtual SPERESULT Initialize(BYTE *pVertex, int StrideSize, int *pIndex, int NumTriangles) = 0; // create a shape from Tri-Mesh, index is 32BIT
	virtual SPERESULT Initialize(BYTE *pVertex, int StrideSize, WORD *pIndex, int NumTriangles) = 0; // index is 16BIT
	virtual SPERESULT InitializeForStatic(BYTE *pVertex, int StrideSize, int *pIndex, int NumTriangles) = 0; // create a shape for static objects
	virtual SPERESULT Initialize(BYTE *pVertex, int StrideSize, int NumVertices) = 0; // create a convex hull by giving vertices
	virtual SPERESULT Initialize(SPEMesh &speMesh) = 0;
	virtual SPERESULT InitializeAsSphere(float r) = 0;
	virtual SPERESULT InitializeAsCapsule(float length, float r) = 0;
	virtual float GetVolume() = 0;
	virtual SPERESULT SetMassCenter(const SPEVector &cm) = 0;
	virtual SPEVector GetMassCenter() = 0;
	virtual SPEMesh* GetMesh() = 0;
	virtual SPERESULT CloneTo(ISPEShape** ppIShape) = 0;   // destination must be a NULL ISPEShape pointer

protected:
	ISPEShape(){}
	virtual ~ISPEShape(){}
};

struct ISPEPhysicsState
{
	// Set or Get member of State for body frame
	virtual void SetPosition(const SPEVector pos) = 0;
	virtual SPEVector GetPosition() = 0;
	virtual void SetOrientation(const SPEMatrix ort) = 0;
	virtual SPEMatrix GetOrientation() = 0;
	virtual void SetVelocity(const SPEVector vel) = 0;
	virtual SPEVector GetVelocity() = 0;
	virtual void SetAngularVelocity(const SPEVector& av) = 0;
	virtual SPEVector GetAngularVelocity() = 0;

	// Set or Get member of State for mesh frame
	virtual void SetPositionMesh(const SPEVector pos) = 0;
	virtual SPEVector GetPositionMesh() = 0;
	virtual void SetOrientationMesh(const SPEMatrix ort) = 0;
	virtual SPEMatrix GetOrientationMesh() = 0;

	// Transform a point or a vector between body frame and world frame
	virtual SPEVector PointBodyToWorld(const SPEVector &p) = 0;
	virtual SPEVector PointWorldToBody(const SPEVector &p) = 0;
	virtual SPEVector VectorBodyToWorld(const SPEVector &p) = 0;
	virtual SPEVector VectorWorldToBody(const SPEVector &p) = 0;

	// Get the velocity at a point in world frame
	virtual SPEVector GetVelocityAtWorldPoint(const SPEVector &p) = 0;

protected:
	ISPEPhysicsState(){}
	virtual ~ISPEPhysicsState(){}
};


struct ISPERigidBody
{
	// Mass
	virtual void SetMass(float mass) = 0; //
	virtual float GetMass() = 0;
	virtual void SetDensity(float density=1000) = 0; // 1000 is the default
	virtual float GetDensity() = 0;
	virtual void SetInertia(const SPEMatrix &inertia) = 0;
	virtual SPEMatrix GetInertia() = 0;
	virtual SPEMatrix GetWorldInertia() = 0;	

	// Physics state
	virtual ISPEPhysicsState* GetState() = 0;
	virtual ISPEPhysicsState* GetNewState() = 0;

	// the following "Set..." method would apply to both "State" and "NewState"
	virtual void SetPosition(const SPEVector& pos) = 0;
	virtual SPEVector GetPosition() = 0;
	virtual void SetOrientation(const SPEMatrix& ort) = 0;
	virtual SPEMatrix GetOrientation() = 0;
	virtual void SetVelocity(const SPEVector& v) = 0;
	virtual SPEVector GetVelocity() = 0;
	virtual void SetAngularVelocity(const SPEVector& av) = 0;
	virtual SPEVector GetAngularVelocity() = 0;

	virtual void SetPositionMesh(const SPEVector& pos) = 0;
	virtual SPEVector GetPositionMesh() = 0;
	virtual void SetOrientationMesh(const SPEMatrix& ort) = 0;
	virtual SPEMatrix GetOrientationMesh() = 0;
	virtual void SetTransformMesh(const SPEMatrix& trans) = 0;
	virtual void SetTransformMesh(void* pmat) = 0;
	virtual SPEMatrix GetTransformMesh(void* pmat=0) = 0;

	// Properties
	virtual void SetBeStatic(bool bs) = 0;  // set if the body be static
	virtual bool GetBeStatic() = 0;
	virtual SPERESULT SetSleeping(bool sleeping) = 0;
	virtual bool GetSleeping() = 0;
	virtual void SetSleepFactor(float factor) = 0;
	virtual float GetSleepFactor() = 0;
	virtual void SetDynamicGeneration(bool dg) = 0; // if this is true, Velocity and AngularVelocity is
	//generated while you call SetTransform or SetPosition or SetOrientation, making ragdoll interact with other bodies.
	virtual bool GetDynamicGeneration() = 0;
	virtual void SetExternForceField(const SPEVector &force) = 0; // this function can used for particles
	virtual SPEVector GetExternForceField() = 0;

	// Materials
	virtual void SetDamping(float damping) = 0;
	virtual float GetDamping() = 0;
	virtual void SetAngularDamping(float angulardamping) = 0;
	virtual float GetAngularDamping() = 0;
	virtual void SetFriction(float friction) = 0;
	virtual float GetFriction() = 0;
	virtual void SetElasticity(float elasticity) = 0;
	virtual float GetElasticity() = 0;

	// interaction
	virtual void ApplyWorldImpulse(const SPEVector &J, const SPEVector &p) = 0;
	virtual void ApplyBodyTorque(const SPEVector &torque) = 0;

	// Contact query
	virtual int	  GetNumContacts() = 0;
	virtual ISPEContact* GetContact(int index) = 0;
	virtual bool CastRay(const SPEVector &RayPos, const SPEVector &RayDir, SPECastRayInfo &Info) = 0; // test if a ray intersect this body

	/////////////////// Physics filter
	// first way to filter collision
	virtual void AddCollisionException(ISPERigidBody *pIBody) = 0;
	virtual bool BeInCollisionException(ISPERigidBody *pIBody) = 0;
	virtual void DeleteCollisionException(ISPERigidBody *pIBody=0) = 0;
	// second way to filter collision and solving
	virtual void SetGroupId(int id) = 0;
	virtual int GetGroupId() = 0;
	// and then use ISPEWorld::AddGroupPhysicsFlag() to filter collisions and solving


	// Breakable object
	virtual void SetBreakForce(float force) = 0;
	virtual float GetBreakForce() = 0;
	virtual bool WantBreak() = 0;
	virtual ISPEContact* GetBreakContact() = 0;
	virtual void PatternState(ISPERigidBody* ibody) = 0;

	virtual ISPEShape* GetShape() = 0;

	void*		UserData;
	int			iUserData;
	float		fUserData;

protected:
	ISPERigidBody()
	{
		UserData=0;
		iUserData=0;
		fUserData=0.0f;
	}
	virtual ~ISPERigidBody(){}
};

struct ISPEJoint
{
	virtual ISPERigidBody* GetRigidBody(int index) = 0;
	virtual SPEVector GetWorldAnchor() = 0;
	virtual void SetBodyAnchor(int index, const SPEVector &anchor) = 0;
	virtual SPEVector GetBodyAnchor(int index) = 0;
	virtual bool IsBroken() = 0;
	virtual ISPEContact* GetContact() = 0;

protected:
	ISPEJoint(){}
	virtual ~ISPEJoint(){}
};

struct ISPEConstraint
{
	virtual SPERESULT SetData(const SPEConstraintData &data) = 0;
	virtual void GetData(SPEConstraintData &data) = 0;
	virtual void SetEnabled(bool enabled) = 0;
	virtual bool GetEnabled() = 0;	
	virtual SPEVector GetPosition() = 0;
	virtual SPEVector GetNormal() = 0;
	virtual SPEVector GetImpulse() = 0;
	
protected:
	ISPEConstraint(){}
	virtual ~ISPEConstraint(){}
};

struct ISPEContact
{
	virtual void SetRigidBody(ISPERigidBody* pIBody0, ISPERigidBody* pIBody1) = 0;
	virtual ISPERigidBody* GetRigidBody(int index) = 0; // index must be 0 or 1
	virtual ISPEConstraint* AddConstraint() = 0;
	virtual SPERESULT DeleteConstraint(ISPEConstraint* pIConstraint=0) = 0;
	virtual SPERESULT SetEnabled(bool enabled) = 0;
	virtual bool GetEnabled() = 0;
	virtual int GetNumConstraints() = 0;
	virtual ISPEConstraint* GetConstraint(int index) = 0;
	virtual ISPEConstraint* GetVirtualConstraint() = 0;
	virtual SPECONTACTTYPE GetType() = 0;
	virtual SPEVector GetSumImpulse(int index) = 0; // index must be 0 or 1
	virtual SPEVector GetSumTorque(int index) = 0; // index must be 0 or 1

protected:
	ISPEContact(){}
	virtual ~ISPEContact(){}
};

struct ISPEParticle
{
	virtual void SetPosition(const SPEVector& pos) = 0;
	virtual SPEVector GetPosition(void* pos=0) = 0;
	virtual void SetVelocity(const SPEVector& v) = 0;
	virtual SPEVector GetVelocity(void* v=0) = 0;

protected:
	ISPEParticle(){}
	virtual ~ISPEParticle(){}
};

struct ISPEFluid
{
	virtual void SetMaterial(SPEFLUIDMATERIAL material) = 0;
	virtual ISPEParticle* AddParticle(const SPEVector &position) = 0;
	virtual ISPEParticle* AddParticle(const SPEVector &position, const SPEVector &velocity) = 0;
	virtual void DeleteParticle(ISPEParticle* pIParticle=0) = 0;
	virtual int GetNumParticles() = 0;
	virtual void GetParticlePosition(void* pData, int StrideSize) = 0;
	virtual void GetParticleVelocity(void* pData, int StrideSize) = 0;
	virtual void SetMeshGeneration(bool mg) = 0;
	virtual bool GetMeshGeneration() = 0;
	virtual SPEMesh* GetMesh() = 0;

protected:
	ISPEFluid(){}
	virtual ~ISPEFluid(){}
};

struct ISPEWorld
{
	virtual void SetGravity(const SPEVector &g) = 0;
	virtual SPEVector GetGravity() = 0;

	// Update and Step
	virtual void SetStepTime(float time=0.01f) = 0; // 0.01 is the default
	virtual float GetStepTime() = 0;
	virtual void SetMaxStepPerUpdate(int nStep=2) = 0; // 2 is the default
	virtual int GetMaxStepPerUpdate() = 0;
	virtual int Update(float dt) = 0;
	virtual void DirectStep() = 0;

	// Solver
	virtual void SetSolverCacheFactor(float factor=0.2f) = 0;  // 0.2 is the default
	virtual float GetSolverCacheFactor() = 0;
	virtual void SetSolverPrecision(int precision=2) = 0; // 2 is the default
	virtual int GetSolverPrecision() = 0;
	
	// Call Back functions
	virtual void SetCallBackPreSolve(SPECallBackFunc pFunc, void *pParam) = 0;
	virtual void SetCallBackStepFinished(SPECallBackFunc pFunc, void *pParam) = 0;

	// Multi-Threads
#ifdef SPE_MT_LIB
	virtual void SetNumThreads(int n) = 0;
	virtual int GetNumThreads() = 0;
#endif

	// Shape
	virtual ISPEShape*		CreateShape() = 0;
	virtual void					ReleaseShape(ISPEShape *pIShape) = 0;

	// Simulation stuff
	virtual ISPERigidBody*	AddRigidBody(ISPEShape *pIShape=0) = 0; // if pIShape==0, member "Shape" is used
	virtual SPERESULT		DeleteRigidBody(ISPERigidBody *pIBody=0) = 0; // if pIBody==0, all the bodies would be deleted
	virtual ISPEFluid*			AddFluid() = 0;
	virtual SPERESULT		DeleteFluid(ISPEFluid* pIFluid=0) = 0;
	virtual ISPEJoint*			AddJoint(SPEJointDesc& joint) = 0;
	virtual SPERESULT		DeleteJoint(ISPEJoint* pIJoint=0) = 0; // if pIJoint==0, all the joints would be deleted
	virtual ISPEContact*		AddCustomContact() = 0;
	virtual SPERESULT		DeleteCustomContact(ISPEContact* pIContact=0) = 0;

	virtual void Clear() = 0; // clear all above stuff
	virtual void ClearGarbage() = 0; // release all unused memory


	// Individual collision detection
	virtual ISPERigidBody* CreateRigidBody(ISPEShape* pIShape=0) = 0; // if pIShape==0, member "Shape" is used
	virtual SPERESULT ReleaseRigidBody(ISPERigidBody *pIBody) = 0;
	virtual void DetectCollisions(SPEArray<ISPERigidBody*> &RigidBodyList) = 0;

	// Physics filter
	virtual void AddGroupPhysicsFlag(int id0, int id1, SPEGROUPPHYSICSFLAG flag) = 0;
	virtual SPEGROUPPHYSICSFLAG GetGroupPhysicsFlag(int id0, int id1) = 0;
	virtual void DeleteGroupPhysicsFlag(int id0, int id1) = 0;
	virtual int GetNumGroupPhysicsFlags() = 0;
	virtual void ClearGroupPhysicsFlag() = 0;

	// for breakable rigid bodies
	virtual int GetBreakList() = 0;
	virtual void Carve(ISPERigidBody* ibody, SPEPlane &knife) = 0;
	virtual void Carve(ISPERigidBody* ibody, SPEArray<SPEPlane> &knives) = 0;

	virtual bool CastRay(const SPEVector &RayPos, const SPEVector &RayDir, SPECastRayInfo &Info) = 0;  // find the nearest intersection


	// internal Shape instance
	ISPEShape*	Shape;

	// Output data, do NOT use for other purpose	
	SPEArray<SPEMesh> Meshes;
	SPEArray<ISPERigidBody*> List;

protected:
	ISPEWorld(){}
	virtual ~ISPEWorld(){}
};

typedef struct ISPEShape			*LPSPESHAPE;
typedef struct ISPEPhysicsState			*LPSPEPHYSICSSTATE;
typedef struct ISPERigidBody		*LPSPERIGIDBODY;
typedef struct ISPEJoint			*LPSPEJOINT;
typedef struct ISPEConstraint			*LPSPECONSTRAINT;
typedef struct ISPEContact			*LPSPECONTACT;
typedef struct ISPEParticle		*LPSPEPARTICLE;
typedef struct ISPEFluid			*LPSPEFLUID;
typedef struct ISPEWorld			*LPSPEWORLD;


SPE_API ISPEWorld*		CreateSPEWorld();
SPE_API void				ReleaseSPEWorld(ISPEWorld* pIWorld);


#endif

