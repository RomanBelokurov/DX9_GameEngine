#ifndef __LGS_QUAD_TREE__H__
#define __LGS_QUAD_TREE__H__

#include "LHeader\LIGA_Object.h"


# define QUADTREE_HI 200.0f
# define QUADTREE_LO -10.0f

struct vertex
{
	D3DXVECTOR3 pos;
	float u,v;
};


void SortObjects_QTree();
void SortObjectsInLocal_QTree();

struct CHUNK
{
//Terrain
	LPD3DXMESH Land_Chunk;
	void ComputeBoundingBox(LPDIRECT3DDEVICE9 g_pd3dDevice);
	AABB ChunkAABB;	

//Objects
	AABB Box;
	vector<DWORD> OBJECT_ID;
	void RenderCurrentChunk(bool DrawShadows=true);
	void RenderCurrentChunkFS();
};


void CHUNK::ComputeBoundingBox(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	BYTE* v = 0;
	if(Land_Chunk!=NULL)
	{
		Land_Chunk->LockVertexBuffer(0, (void**)&v);
		D3DXComputeBoundingBox((D3DXVECTOR3*)v,Land_Chunk->GetNumVertices(),D3DXGetFVFVertexSize(Land_Chunk->GetFVF()),&ChunkAABB.minPt,&ChunkAABB.maxPt);
		Land_Chunk->UnlockVertexBuffer();
	}
	Box.maxPt = ChunkAABB.maxPt;
	Box.minPt = ChunkAABB.minPt;

	Box.maxPt.y = QUADTREE_HI;
	Box.minPt.y = QUADTREE_LO;
}

CHUNK LAND_CHUNK[50][50];

int TERRAIN_SCALE=0;
int TERRAIN_FOV=2;

void SortObjectsInQuads()
{
	if(OBJ_COUNTER>0)
	{
		for(int i=0;i<TERRAIN_SCALE;i++)
			for(int j=0;j<TERRAIN_SCALE;j++)
			{
				LAND_CHUNK[i][j].OBJECT_ID.clear();
				for(int o=0; o<OBJ_COUNTER; o++)
				{
				   if(LAND_CHUNK[i][j].Box.PointInBox(&OBJECT_VECTOR[o].Position))LAND_CHUNK[i][j].OBJECT_ID.push_back(o);
				}
			}
	}
}

void CHUNK::RenderCurrentChunk(bool DrawShadows)
{
	for(int i=0;i<(int)OBJECT_ID.size();i++)
	{
		if(VP1Camera.isVisible(OBJECT_VECTOR[OBJECT_ID[i]].New.minPt,OBJECT_VECTOR[OBJECT_ID[i]].New.maxPt))
			OBJECT_VECTOR[OBJECT_ID[i]].DrawWithLight(VPORT1,VP1Camera,RM,DrawShadows);
	}
}


void CHUNK::RenderCurrentChunkFS()
{
	for(int i=0;i<(int)OBJECT_ID.size();i++)
	{
	 	if(OBJECT_VECTOR[OBJECT_ID[i]].DropShadow)OBJECT_VECTOR[OBJECT_ID[i]].DrawForShadowMap(VPORT1,VP1Camera,RM);
	}
}

#endif  __LGS_QUAD_TREE__H__
