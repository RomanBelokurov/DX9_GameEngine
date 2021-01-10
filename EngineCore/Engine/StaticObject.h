#ifndef _SOLIDOBJECT_H_
#define _SOLIDOBJECT_H_
#pragma once


#include	<d3dx9math.h>
#include	<iostream>
#include	<vector>
#include	"ResourceManager.h"

// This is old implementation!
class StaticObject {
public:
	DWORD MeshID;
	std::vector<DWORD> TextureLayer_0;

	StaticObject() {}
	~StaticObject() {}

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetScale(float, float, float);
	
	void Update()
	{
		D3DXMATRIX mm1, mm2, mm3, mrot, mResc, mTrans;
		D3DXVECTOR3 _x(1.0f, 0.0f, 0.0f);
		D3DXVECTOR3 _y(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 _z(0.0f, 0.0f, 1.0f);

		D3DXMatrixTranslation(&mTrans, Position.x, Position.y, Position.z);
		D3DXMatrixRotationAxis(&mm1, &_x, Rotation.x * 0.0174532925f);
		D3DXMatrixRotationAxis(&mm2, &_y, Rotation.y * 0.0174532925f);
		D3DXMatrixRotationAxis(&mm3, &_z, Rotation.z * 0.0174532925f);
		mrot = mm3 * mm1 * mm2;
		D3DXMatrixScaling(&mResc, Scalling.x, Scalling.y, Scalling.z);
		World = mrot * mResc * mTrans;
		//Now.xform(World, New);
	}

public:
	D3DXMATRIX World;
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Scalling;
	D3DXVECTOR3 Rotation;
};



#endif