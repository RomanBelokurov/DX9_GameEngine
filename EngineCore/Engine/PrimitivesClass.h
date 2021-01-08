#pragma once

#ifndef _PRIMITIVES_CLASS_H_
#define _PRIMITIVES_CLASS_H_

#include	<d3dx9.h>
#include	<d3d9.h>

class PrimitivesClass
{
public:
	static void DrawLine(IDirect3DDevice9* m_device, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DCOLOR color);
	static void DrawGrid(IDirect3DDevice9* m_device, D3DCOLOR color);
};

void PrimitivesClass::DrawLine(IDirect3DDevice9* m_device, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DCOLOR color)
{
	struct 
	{ 
		D3DXVECTOR3 coord;
		D3DCOLOR color; 
	}verticies[2] = { p1, color,p2, color };

	m_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	m_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, verticies, sizeof(verticies[0]));
}

void PrimitivesClass::DrawGrid(IDirect3DDevice9* m_device, D3DCOLOR color)
{
	for (int i = -25; i <= 25; i++)
	{
		DrawLine(m_device, D3DXVECTOR3(-500.0, 0.0, 20 * i), D3DXVECTOR3(500.0, 0.0, 20 * i), color);
		DrawLine(m_device, D3DXVECTOR3(20 * i, 0.0, -500.0), D3DXVECTOR3(20 * i, 0.0, 500.0), color);
	}
}

#endif