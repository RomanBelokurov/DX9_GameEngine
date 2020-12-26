#pragma once

#ifndef __DRIVER_DX9__H__
#define __DRIVER_DX9__H__

#include	<d3dx9.h>
#include	<d3d9.h>
#include	<d3dx9math.h>
#include	<d3dx9mesh.h>
#include	<cassert>

struct RenderConfig
{
	float	screenWidth;
	float	screenHeight;
	float	refreshRate;
	bool	dynamicSky;
	bool	postProcess;
	bool	RenderShadows;
	bool	DrawParticles;

	RenderConfig()
	{
		screenWidth = 1024;
		screenHeight = 768;
		refreshRate = 75;
		dynamicSky = false;
		postProcess = true;
		RenderShadows = false;
		DrawParticles = true;
	}
};


class __declspec(dllexport) Driver_DX9
{
	//public fields
	public:
		RenderConfig RConf;
		LPDIRECT3DDEVICE9 VPORT1;
	
	//public Methods
	public:

		bool InitDriver(HWND hWnd);

		void EnableAlpha();
		void DisableAlpha();

		void Begin(DWORD param, D3DCOLOR color);
		void End();

		void SetFog(D3DCOLOR color, float fFogStart, float fFogEnd);
};






#endif