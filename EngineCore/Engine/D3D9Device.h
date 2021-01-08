#pragma once

#ifndef __D3D9DEVICE__H__
#define __D3D9DEVICE__H__

#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"Vfw32.lib")


#include	<d3dx9.h>
#include	<d3d9.h>
#include	<d3dx9math.h>
#include	<d3dx9mesh.h>
#include	<cassert>


class D3D9Device
{
public:
	D3D9Device();
	~D3D9Device();

	bool Initialize(HWND, int, int, bool, float, float, float);
	void Shutdown();

	void BeginScene(int, int, int, int);
	void EndScene();

	IDirect3DDevice9* GetDevice() { return m_device; }

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	int GetScreenWidth() { return m_screenWidth; }
	int GetScreenHeight() { return m_screenHeight; }

private:
	IDirect3DDevice9* m_device;

	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;

	int m_screenWidth;
	int m_screenHeight;
};

D3D9Device::D3D9Device()
{
	m_device = 0; 
	m_screenWidth = 0;
	m_screenHeight = 0;
}

D3D9Device::~D3D9Device()
{
}

bool D3D9Device::Initialize(HWND hWnd, int screenWidth, int screenHeight, bool fullscreen, float projectionFOV, float screenDepth, float screenNear)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	LPDIRECT3D9				g_pD3D;
	D3DPRESENT_PARAMETERS	g_D3Dpp;

	if (!(g_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) exit(0);

	ZeroMemory(&g_D3Dpp, sizeof(g_D3Dpp));

	g_D3Dpp.BackBufferWidth = m_screenWidth;
	g_D3Dpp.BackBufferHeight = m_screenHeight;

	D3DDISPLAYMODE d3ddm;
	memset(&d3ddm, 0, sizeof(D3DDISPLAYMODE));
	g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	g_D3Dpp.BackBufferFormat = d3ddm.Format;

	D3DFORMAT dispFormat[] = { D3DFMT_D32,D3DFMT_D24S8,D3DFMT_D24X4S4,D3DFMT_D24X8,D3DFMT_D16 };
	for (size_t j = 0; j < sizeof(dispFormat) / sizeof(dispFormat[0]); ++j)
	{
		if (FAILED(g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, dispFormat[j]))) continue;
		g_D3Dpp.AutoDepthStencilFormat = dispFormat[j];
		break;
	}

	g_D3Dpp.Windowed = !fullscreen;
	g_D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//RConf.refreshRate;
	g_D3Dpp.hDeviceWindow = hWnd;
	g_D3Dpp.EnableAutoDepthStencil = TRUE;
	g_D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_D3Dpp, &m_device)))
	{
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_D3Dpp, &m_device)))
		{
			g_pD3D->Release();
			exit(0);
		}
	}

	// Setup the projection matrix.
	float fieldOfView = (float)projectionFOV * 0.0174532925f; //(float)D3DX_PI / 4.0f;
	float screenAspect = (float)m_screenWidth / (float)m_screenHeight;

	// Create the projection matrix for 3D rendering.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	// Initialize the world matrix to the identity matrix.
	D3DXMatrixIdentity(&m_worldMatrix);

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)m_screenWidth, (float)m_screenHeight, screenNear, screenDepth);		


	m_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	m_device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	g_pD3D->Release();
	return true;
}

void D3D9Device::Shutdown() 
{
	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}
}


void D3D9Device::BeginScene(int red, int green, int blue, int alpha)
{
	m_device->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB(alpha, red, green, blue), 1.0f, 0);
	m_device->BeginScene();
}

void D3D9Device::EndScene() 
{
	m_device->EndScene();
	m_device->Present(NULL, NULL, NULL, NULL);
}


void D3D9Device::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3D9Device::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3D9Device::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}


void D3D9Device::TurnZBufferOn() 
{
	m_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

void D3D9Device::TurnZBufferOff() 
{
	m_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
}


void D3D9Device::TurnOnAlphaBlending() 
{
	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_device->SetRenderState(D3DRS_ALPHAREF, 0xCC);
	m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}


void D3D9Device::TurnOffAlphaBlending() 
{
	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_device->SetRenderState(D3DRS_ALPHAREF, 0xCC);
	m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

#endif