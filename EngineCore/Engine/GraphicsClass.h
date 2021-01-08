#pragma once

#ifndef __GRAPHICS_CLASS__H__
#define __GRAPHICS_CLASS__H__

#include "D3D9Device.h"
#include "CameraClass.h"
#include "FrustumClass.h"
#include "PrimitivesClass.h"
#include "TextClass.h"

const bool FULL_SCREEN = false;
const float CAMERA_FOV = 45.0f;

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
	public:
		GraphicsClass();
		~GraphicsClass();

		bool Initialize(HWND hWnd, int screenWidth, int screenHeight);
		void Shutdown();
		
		bool Update();
		bool Render();

		CameraClass* GetCamera() { return m_Camera; }
	
	private:
		D3D9Device* m_D3D;
		CameraClass* m_Camera;
		FrustumClass* m_Frustum;
		TextClass* m_Text;
};

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Frustum = 0;
	m_Text = 0;
}

GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(HWND hWnd, int screenWidth, int screenHeight)
{
	m_D3D = new D3D9Device;
	if (!m_D3D)
		return false;

	// Initialize the Direct3D object.
	if(!m_D3D->Initialize(hWnd, screenWidth, screenHeight, FULL_SCREEN, CAMERA_FOV, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hWnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}


	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	m_Camera->SetPosition(0.0f, 30.0f, 0.0f);


	m_Frustum = new FrustumClass;
	if (!m_Frustum)
		return false;

	m_Text = new TextClass;
	if (!m_Text)
		return false;
	m_Text->Initialize(m_D3D->GetDevice(), "Arial", 20, FW_BOLD);

	return true;
}


bool GraphicsClass::Update()
{
	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;

	m_D3D->BeginScene(50, 30, 50, 255);

	// Apply transformations
	m_Camera->Update();
	m_Camera->GetViewMatrix(viewMatrix);

	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	
	D3DXMATRIX mViewProj = viewMatrix * projectionMatrix;
	m_D3D->GetDevice()->SetTransform(D3DTS_PROJECTION, &projectionMatrix);
	m_D3D->GetDevice()->SetTransform(D3DTS_VIEW, &viewMatrix);
	m_D3D->GetDevice()->SetTransform(D3DTS_WORLD, &worldMatrix);
		

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	PrimitivesClass::DrawGrid(m_D3D->GetDevice(), D3DCOLOR_XRGB(200, 0, 0));
	
	m_D3D->TurnZBufferOff();
	m_Text->Render("Text", 75, m_D3D->GetScreenHeight() * 0.93f, D3DCOLOR_XRGB(0, 155, 255));
	m_D3D->TurnZBufferOn();
	m_D3D->EndScene();

	return true;
}

void GraphicsClass::Shutdown()
{
	/*
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}*/

	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}
	
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
}

#endif