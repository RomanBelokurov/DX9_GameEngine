#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_
#pragma once


#include	<d3dx9math.h>

typedef struct
{
	D3DXVECTOR3 coord;
	D3DCOLOR color;
}bbox_vert;

struct Vertex
{
	float x, y, z;
	DWORD diffuse;
};

class BoundingBox {
	public:
		BoundingBox();
		
		void ComputeBoundingBox();
		void RenderBoundingBox();
};

#endif