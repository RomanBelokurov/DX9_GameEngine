#pragma once
#ifndef __APPCLASS__
#define __APPCLASS__

#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>

#include "TimerClass.h"
#include "GraphicsClass.h"
#include "InputClass.h"
#include "PlayerClass.h"

class AppClass
{	
public:
	AppClass();
	~AppClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	void InitializeWindow(int&, int&);
	void ShutdownWindow();

	bool HandleInput(float);

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	GraphicsClass* m_Graphics;
	TimerClass* m_Timer;
	InputClass* m_Input;
	PlayerClass* m_Player;

private:
	int screenWidth = 0;
	int screenHeight = 0;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static AppClass* ApplicationHandle = 0;


AppClass::AppClass()
{
	m_Graphics = 0;
	m_Timer = 0;
	m_Input = 0;
	m_Player = 0;
}

AppClass::~AppClass()
{
}


bool AppClass::Initialize()
{
	// Initialize the windows api.
	InitializeWindow(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if (!m_Input)
		return false;

	// Initialize the input object.
	if(!m_Input->Initialize(m_hinstance, m_hwnd))
		return false;


	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
		return false;

	// Initialize the graphics object.
	if(!m_Graphics->Initialize(m_hwnd, screenWidth, screenHeight))
		return false;


	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer)
		return false;

	// Initialize the timer object.
	if(!m_Timer->Initialize())
		return false;

	// Player class intialize at camera position
	D3DXVECTOR3 cameraPosition = m_Graphics->GetCamera()->GetPosition();
	D3DXVECTOR3 cameraRotation = m_Graphics->GetCamera()->GetRotation();
	m_Player = new PlayerClass(cameraPosition, cameraRotation);
	if (!m_Player)
		return false;

	return true;
}

void AppClass::Shutdown()
{
	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the graphics object.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	if (m_Player)
	{
		delete m_Player;
		m_Player = 0;
	}

	// Shutdown the window.
	ShutdownWindow();

	return;
}

bool AppClass::Update()
{
	m_Timer->Update();

	if(!m_Input->Update())
		return false;

	if (!HandleInput(m_Timer->GetTime()))
		return false;

	
	// Graphics update => render
	if (!m_Graphics->Update())
		return false;

	if(!m_Graphics->Render())
		return false;

	return true;
}

void AppClass::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.  If frame processing fails then exit.
			result = Update();
			if (!result)
			{
				MessageBox(m_hwnd, "Frame Processing Failed", "Error", MB_OK);
				done = true;
			}
		}

		// Check if the user pressed escape and wants to quit.
		if (m_Input->IsKeyButtonPressed(DIK_ESCAPE) == true)
			done = true;
	}

	return;
}


bool AppClass::HandleInput(float frameTime)
{
	bool keyDown, result;
	D3DXVECTOR3 camPosition = m_Graphics->GetCamera()->GetPosition();
	D3DXVECTOR3 camRotation = m_Graphics->GetCamera()->GetRotation();

	keyDown = m_Input->IsKeyButtonPressed(DIK_A);
	if (keyDown) m_Player->StrafeLeft(frameTime);

	keyDown = m_Input->IsKeyButtonPressed(DIK_D);
	if (keyDown) m_Player->StrafeRight(frameTime);
		
	// Forward
	keyDown = m_Input->IsKeyButtonPressed(DIK_W);
	if (keyDown) m_Player->MoveForward(frameTime);
	
	// Backward
	keyDown = m_Input->IsKeyButtonPressed(DIK_S);
	if (keyDown) m_Player->MoveBackward(frameTime);
	

	int dx, dy;
	m_Input->GetMouseLocationDiff(dx, dy);
	m_Player->LookAround(frameTime, dx, dy);
	
	D3DXVECTOR3 playerPosition = m_Player->GetPlayerPosition();
	D3DXVECTOR3 playerRotation = m_Player->GetPlayerRotation();

	m_Graphics->GetCamera()->SetPosition(playerPosition.x, playerPosition.y, playerPosition.z);
	m_Graphics->GetCamera()->SetRotation(playerRotation.x, playerRotation.y, playerRotation.z);

	return true;
}




LRESULT CALLBACK AppClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void AppClass::InitializeWindow(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Game engine";
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, "Game engine", "Game engine",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}


void AppClass::ShutdownWindow()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass("Game engine", m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}


#endif