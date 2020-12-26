#ifndef __LGS_OBJECT__H__
#define __LGS_OBJECT__H__

#include	<math.h>
#include	<windows.h>
#include	<mmsystem.h>
#include	<d3dx9.h>
#include	<d3d9.h>
#include	<windowsx.h>
#include	<d3dx9math.h>
#include	<d3dx9mesh.h>
#include	<stdio.h>
#include 	<vector>
using namespace std;

#include	"LIGA_Sky.h"

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }
#define V3 D3DXVECTOR3
#define DEG_TO_RAD(ang) ((ang)*D3DX_PI/180)


////////////////////////////////////////////////




typedef struct
{
	V3 coord;
	D3DCOLOR color;
}bb_vert;

struct Vertex
{
	float x, y, z;
	DWORD diffuse;
};

void DrawBB(LPDIRECT3DDEVICE9 g_pd3dDevice,V3 min,V3 max,D3DCOLOR color)
{
	g_pd3dDevice->SetTexture( 0, NULL );
	bb_vert vert[24]=
	{
		min, color,V3(max.x,min.y,min.z), color,
		V3(max.x,min.y,min.z), color,V3(max.x,min.y,max.z), color,
		V3(max.x,min.y,max.z), color,V3(min.x,min.y,max.z), color,
		V3(min.x,min.y,max.z), color,min, color,

		V3(min.x,max.y,min.z), color,V3(max.x,max.y,min.z), color,
		V3(max.x,max.y,min.z), color,V3(max.x,max.y,max.z), color,
		V3(max.x,max.y,max.z), color,V3(min.x,max.y,max.z), color,
		V3(min.x,max.y,max.z), color,V3(min.x,max.y,min.z), color,

		V3(min.x,max.y,min.z), color,min, color,
		V3(max.x,max.y,min.z), color,V3(max.x,min.y,min.z), color,
		V3(max.x,max.y,max.z), color,V3(max.x,min.y,max.z), color,
		V3(min.x,max.y,max.z), color,V3(min.x,min.y,max.z), color
	};
	g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST,12, vert, sizeof(vert[0]));
}



class Object
{
public:
	DWORD MeshID;
	vector<DWORD> TextureLayer_0;

	D3DXMATRIX World;
	V3 Position,Scalling,Rotation;

	AABB 	Now,New;
	BOOL 	SELECTED;
	BOOL 	beIndependent;
	BOOL 	Colide;
	DWORD 	FULL_CULLING;
	BOOL 	DropShadow;

	Object()
	{
		TextureLayer_0.clear();
		FULL_CULLING		=0;
		MeshID				=0;
		Position			=V3(0.0f,0.0f,0.0f);
		Scalling			=V3(1.0f,1.0f,1.0f);
		Rotation			=V3(0.0f,0.0f,0.0f);
		SELECTED			=FALSE;
		Colide 				=TRUE;
		beIndependent		=FALSE;
		DropShadow			=FALSE;
	}
	void ComputeBoundingBox(LPDIRECT3DDEVICE9 g_pd3dDevice,ResourceManager RM);
	void UpdatePosAABB();
	void DrawWithLight(LPDIRECT3DDEVICE9 g_pd3dDevice,DXCamera ActiveCamera,ResourceManager RM, bool DrawShadows=false);
    void DrawForShadowMap(LPDIRECT3DDEVICE9 g_pd3dDevice,DXCamera ActiveCamera,ResourceManager RM);
};



void Object::DrawForShadowMap(LPDIRECT3DDEVICE9 g_pd3dDevice,DXCamera ActiveCamera,ResourceManager RM)
{
	if(FULL_CULLING==1)g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);else g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &World);
	SKYDOME.Update();
	for( DWORD i=0; i<RM.MATNUM[MeshID]; i++ )
	{
		if(RM.RM_MESH[MeshID]!=NULL)RM.RM_MESH[MeshID]->DrawSubset( i );
	}
	D3DXMATRIX Reset;
	D3DXMatrixIdentity(&Reset);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &Reset);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


void Object::DrawWithLight(LPDIRECT3DDEVICE9 g_pd3dDevice,DXCamera ActiveCamera,ResourceManager RM,bool DrawShadows)
{
	if(FULL_CULLING==1)g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);else g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &World);

	if(DrawShadows)
		{
			SKYDOME.Update();
			SKYDOME.m_Effect->Begin( NULL, 0 );
			SKYDOME.m_Effect->BeginPass( 1 );

			for( DWORD i=0; i<RM.MATNUM[MeshID]; i++ )
			{
				SKYDOME.m_Effect->SetTexture( "TexScene", RM.RM_TEXTURE[TextureLayer_0[i]] );
				SKYDOME.m_Effect->CommitChanges();
				if(RM.RM_MESH[MeshID]!=NULL)RM.RM_MESH[MeshID]->DrawSubset( i );
			}
			SKYDOME.m_Effect->EndPass();
			SKYDOME.m_Effect->End();
		}else
		{
			SKYDOME.Update();
			SKYDOME.Lightmap_Effect->Begin( NULL, 0 );
			SKYDOME.Lightmap_Effect->BeginPass( 0 );

			for( DWORD i=0; i<RM.MATNUM[MeshID]; i++ )
			{
				SKYDOME.Lightmap_Effect->SetTexture( "TexScene", RM.RM_TEXTURE[TextureLayer_0[i]] );
				SKYDOME.Lightmap_Effect->CommitChanges();
				if(RM.RM_MESH[MeshID]!=NULL)RM.RM_MESH[MeshID]->DrawSubset( i );
			}
			SKYDOME.Lightmap_Effect->EndPass();
			SKYDOME.Lightmap_Effect->End();
		}
		D3DXMATRIX Reset;
		D3DXMatrixIdentity(&Reset);
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &Reset);
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		g_pd3dDevice->SetTexture( 0, 0);
}

void Object::UpdatePosAABB()
{
	D3DXMATRIX mm1,mm2,mm3,mrot,mResc,mTrans;
	D3DXVECTOR3 _x(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 _y(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 _z(0.0f, 0.0f, 1.0f);

	D3DXMatrixTranslation(&mTrans,Position.x,Position.y,Position.z);
	D3DXMatrixRotationAxis(&mm1, &_x, DEG_TO_RAD(Rotation.x));
	D3DXMatrixRotationAxis(&mm2, &_y, DEG_TO_RAD(Rotation.y));
	D3DXMatrixRotationAxis(&mm3, &_z, DEG_TO_RAD(Rotation.z));
	mrot=mm3*mm1*mm2;
	D3DXMatrixScaling(&mResc,Scalling.x,Scalling.y,Scalling.z);
	World=mrot*mResc*mTrans;
	Now.xform(World, New);
}


void Object::ComputeBoundingBox(LPDIRECT3DDEVICE9 g_pd3dDevice,ResourceManager RM) // ���������� �������������� ��������������
{
	BYTE* v = 0;
	if(RM.RM_MESH[MeshID]!=NULL)
	{
		RM.RM_MESH[MeshID]->LockVertexBuffer(0, (void**)&v);
		D3DXComputeBoundingBox((D3DXVECTOR3*)v,RM.RM_MESH[MeshID]->GetNumVertices(),D3DXGetFVFVertexSize(RM.RM_MESH[MeshID]->GetFVF()),&Now.minPt,&Now.maxPt);
		RM.RM_MESH[MeshID]->UnlockVertexBuffer();
	}
}









vector<Object> OBJECT_VECTOR;
vector<Object> INDEP_OBJ; 
DWORD OBJ_COUNTER=0;
DWORD SELECTED_MESH=0;



void ResortAABB()
{
	vector<Object> Temporary;
	Temporary.clear();
	DWORD minIndex=0;
	float minLength=20000;

	for(int i=0;i<OBJ_COUNTER;i++)
	{
		minLength=20000.0f;
		for(int j=0;j<(int)OBJECT_VECTOR.size();j++)
		{
			if(OBJECT_VECTOR[j].New.GetDiagonal()<minLength){minLength=OBJECT_VECTOR[j].New.GetDiagonal();minIndex=j;}
		}
		Temporary.push_back(OBJECT_VECTOR[minIndex]);
		OBJECT_VECTOR.erase(OBJECT_VECTOR.begin()+minIndex);
	}
	OBJECT_VECTOR=Temporary;
}



void MakeAllObjectsColisional()
{
	
	D3DXMATRIX		mm1,mm2,mm3,mrot,mResc, mat;

	for(int i=0;i<OBJ_COUNTER;i++)
	{
		if(OBJECT_VECTOR[i].Colide)
		{
			//1 Convert to progressive mesh		
			D3DXVECTOR3 _x(1.0f, 0.0f, 0.0f);
			D3DXVECTOR3 _y(0.0f, 1.0f, 0.0f);
			D3DXVECTOR3 _z(0.0f, 0.0f, 1.0f);
			D3DXMatrixRotationAxis(&mm1, &_x, DEG_TO_RAD(OBJECT_VECTOR[i].Rotation.x));
			D3DXMatrixRotationAxis(&mm2, &_y, DEG_TO_RAD(OBJECT_VECTOR[i].Rotation.y));
			D3DXMatrixRotationAxis(&mm3, &_z, DEG_TO_RAD(OBJECT_VECTOR[i].Rotation.z));
			D3DXMatrixScaling(&mResc,OBJECT_VECTOR[i].Scalling.x,OBJECT_VECTOR[i].Scalling.y,OBJECT_VECTOR[i].Scalling.z);
			mrot=mm3*mm1*mm2;
			mat=mrot*mResc;

			LPD3DXMESH		pMesh;	
			LPSPERIGIDBODY	pBody;
			RM.RM_MESH[OBJECT_VECTOR[i].MeshID]->CloneMeshFVF( RM.RM_MESH[OBJECT_VECTOR[i].MeshID]->GetOptions() | D3DXMESH_32BIT, DXFVF, dx9Driver.VPORT1, &pMesh );
			
			D3DXComputeNormals(pMesh, 0);	
			DXVERTEX *dxvb;
			pMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, (void**)&dxvb);
			for(int j=0; j<(int)pMesh->GetNumVertices(); j++)
			{
				D3DXVec3TransformCoord(&dxvb[j].p,&dxvb[j].p,&mat);
			}
			pMesh->UnlockVertexBuffer ();
			
			DWORD *pData;
			pMesh->LockAttributeBuffer (D3DLOCK_NOSYSLOCK, &pData);			
			for(int j=0; j<(int)pMesh->GetNumFaces(); j++)
			{
				pData[j]=0;
			}
			pMesh->UnlockAttributeBuffer ();

			//2 Make a Physical SHAPE
			LPSPESHAPE pShape = pWorld->CreateShape();
			InitShape(pShape,pMesh);
			pBody=pWorld->AddRigidBody (pShape);
			pBody -> SetPositionMesh (SPEVector(OBJECT_VECTOR[i].Position.x, OBJECT_VECTOR[i].Position.y, OBJECT_VECTOR[i].Position.z));
			pBody -> SetBeStatic(true);			
			pBody -> SetElasticity(0.0f);

			pBody -> SetDamping(10000);
			pBody -> SetAngularDamping(10000);
			
			pBody -> SetFriction(0.5f);
			pWorld-> ReleaseShape(pShape);
			
			pMesh->Release();
		}
	}
	
	if(INDEP_OBJ.size()>0)
	{
		D3DXVECTOR3 _x(1.0f, 0.0f, 0.0f);
		D3DXVECTOR3 _y(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 _z(0.0f, 0.0f, 1.0f);
		D3DXMatrixRotationAxis(&mm1, &_x, DEG_TO_RAD(INDEP_OBJ[0].Rotation.x));
		D3DXMatrixRotationAxis(&mm2, &_y, DEG_TO_RAD(INDEP_OBJ[0].Rotation.y));
		D3DXMatrixRotationAxis(&mm3, &_z, DEG_TO_RAD(INDEP_OBJ[0].Rotation.z));
		D3DXMatrixScaling(&mResc,INDEP_OBJ[0].Scalling.x,INDEP_OBJ[0].Scalling.y,INDEP_OBJ[0].Scalling.z);
		mrot=mm3*mm1*mm2;
		mat=mrot*mResc;

		LPD3DXMESH		pMesh;	
		LPSPERIGIDBODY	pBody;
		RM.RM_MESH[INDEP_OBJ[0].MeshID]->CloneMeshFVF( RM.RM_MESH[INDEP_OBJ[0].MeshID]->GetOptions() | D3DXMESH_32BIT, DXFVF, dx9Driver.VPORT1, &pMesh );

		D3DXComputeNormals(pMesh, 0);	
		DXVERTEX *dxvb;
		pMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, (void**)&dxvb);
		for(int j=0; j<(int)pMesh->GetNumVertices(); j++)
		{
			D3DXVec3TransformCoord(&dxvb[j].p,&dxvb[j].p,&mat);
		}
		pMesh->UnlockVertexBuffer ();
		
		DWORD *pData;
		pMesh->LockAttributeBuffer (D3DLOCK_NOSYSLOCK, &pData);			
		for(int j=0; j<(int)pMesh->GetNumFaces(); j++)
		{
			pData[j]=0;
		}
		pMesh->UnlockAttributeBuffer ();

			//2 Make a Physical SHAPE
		LPSPESHAPE pShape = pWorld->CreateShape();
		InitShape(pShape,pMesh);
		pBody=pWorld->AddRigidBody (pShape);
		pBody -> SetPositionMesh (SPEVector(INDEP_OBJ[0].Position.x, INDEP_OBJ[0].Position.y, INDEP_OBJ[0].Position.z));
		pBody -> SetBeStatic(true);
		pBody -> SetFriction(0.7f);
		pWorld-> ReleaseShape(pShape);
			
		pMesh->Release();	
	}
}

#endif __LGS_OBJECT__H__

