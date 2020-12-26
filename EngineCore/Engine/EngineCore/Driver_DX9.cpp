#include "Driver_DX9.h"

#ifdef __cplusplus
extern "C" {
#endif
	extern __declspec(dllexport) Driver_DX9* GetInstance() 
	{
		return new Driver_DX9();
	}

	extern __declspec(dllexport) bool InitDriver(Driver_DX9* a_pObject, HWND hWnd) 
	{
		if (a_pObject != NULL)
		{
			return a_pObject->InitDriver(hWnd);
		}
		return false;
	}

	extern __declspec(dllexport) void Begin(Driver_DX9* a_pObject, DWORD param)
	{
		if (a_pObject != NULL)
		{
			D3DCOLOR color = D3DCOLOR_XRGB(50, 50, 100);
			a_pObject->Begin(param, color);
		}
	}
	
	extern __declspec(dllexport) void End(Driver_DX9* a_pObject)
	{
		if (a_pObject != NULL) a_pObject->End();
	}

#ifdef __cplusplus
}
#endif


bool Driver_DX9::InitDriver(HWND hWnd)
{
	LPDIRECT3D9				g_pD3D;
	D3DPRESENT_PARAMETERS	g_D3Dpp;

	if (!(g_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) exit(0);

	ZeroMemory(&g_D3Dpp, sizeof(g_D3Dpp));

	g_D3Dpp.BackBufferWidth = RConf.screenWidth;
	g_D3Dpp.BackBufferHeight = RConf.screenHeight;

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

	g_D3Dpp.Windowed = true;
	g_D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//RConf.refreshRate;
	g_D3Dpp.hDeviceWindow = hWnd;
	g_D3Dpp.EnableAutoDepthStencil = TRUE;
	g_D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_D3Dpp, &VPORT1)))
	{
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_D3Dpp, &VPORT1)))
		{
			g_pD3D->Release();
			exit(0);
		}
	}

	VPORT1->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	VPORT1->SetRenderState(D3DRS_LIGHTING, FALSE);
	VPORT1->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	VPORT1->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	VPORT1->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	VPORT1->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	VPORT1->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	VPORT1->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	VPORT1->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	EnableAlpha();

	g_pD3D->Release();
	return true;
}

void Driver_DX9::EnableAlpha()
{
	VPORT1->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	VPORT1->SetRenderState(D3DRS_ALPHAREF, 0xCC);
	VPORT1->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	VPORT1->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	VPORT1->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	VPORT1->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void Driver_DX9::DisableAlpha()
{
	VPORT1->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	VPORT1->SetRenderState(D3DRS_ALPHAREF, 0xCC);
	VPORT1->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	VPORT1->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	VPORT1->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	VPORT1->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void Driver_DX9::Begin(DWORD param, D3DCOLOR color)
{	
	VPORT1->Clear(0, NULL, param, color, 1.0f, 0);
	VPORT1->BeginScene();
}


void Driver_DX9::End()
{
	VPORT1->EndScene();
	VPORT1->Present(NULL, NULL, NULL, NULL);
}

void Driver_DX9::SetFog(D3DCOLOR color, float fFogStart, float fFogEnd)
{
	VPORT1->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
	VPORT1->SetRenderState(D3DRS_FOGENABLE, TRUE);
	VPORT1->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	VPORT1->SetRenderState(D3DRS_FOGCOLOR, color);
	VPORT1->SetRenderState(D3DRS_FOGSTART, *((DWORD*)(&fFogStart)));
	VPORT1->SetRenderState(D3DRS_FOGEND, *((DWORD*)(&fFogEnd)));
}