#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#define DIMOUSE_LEFTBUTTON   0
#define DIMOUSE_RIGHTBUTTON  1

class DInputMgr
{
public:
	void InitializeDirectInput(HINSTANCE appInstance,
		HWND      hwnd,
		DWORD keyboardCoopFlags = DISCL_NONEXCLUSIVE|DISCL_FOREGROUND,
		DWORD mouseCoopFlags    = DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	DInputMgr();
	~DInputMgr();

	void poll();

	bool keyDown(char key);

	bool mouseButtonDown(int button);

	double mouseDX();
	double mouseDY();
	double mouseDZ();

private:
	// Make private to prevent copying of members of this class.
	DInputMgr(const DInputMgr& rhs);
	DInputMgr& operator=(const DInputMgr& rhs);

public:
	IDirectInput8*       mDInput;

	IDirectInputDevice8* mKeyboard;
	byte				mKeyboardState[256]; 

	IDirectInputDevice8* mMouse;
	DIMOUSESTATE2        mMouseState;
};

DInputMgr::DInputMgr()
{

}

void DInputMgr::InitializeDirectInput(HINSTANCE appInstance,
					 HWND hwnd,
					 DWORD keyboardCoopFlags,
					 DWORD mouseCoopFlags)
{
	//===============================================================
	// Zero memory.

	ZeroMemory(mKeyboardState, sizeof(mKeyboardState));
	ZeroMemory(&mMouseState, sizeof(mMouseState));

	//===============================================================
	// Init DirectInput.

	DirectInput8Create(
		appInstance, 
		DIRECTINPUT_VERSION,
		IID_IDirectInput8, 
		(void**)&mDInput, 0);

	//===============================================================
	// Init keyboard.

	mDInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, 0);
	mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	mKeyboard->SetCooperativeLevel(hwnd, keyboardCoopFlags);
	mKeyboard->Acquire();

	//===============================================================
	// Init mouse.

	mDInput->CreateDevice(GUID_SysMouse, &mMouse, 0);
	mMouse->SetDataFormat(&c_dfDIMouse2);
	mMouse->SetCooperativeLevel(hwnd, mouseCoopFlags);
	mMouse->Acquire();
}

DInputMgr::~DInputMgr()
{
	mDInput->Release();
	mKeyboard->Release();
	mMouse->Release();
}

void DInputMgr::poll()
{
	//===============================================================
	// Poll keyboard.

	HRESULT hr = mKeyboard->GetDeviceState(sizeof(mKeyboardState), 
		(void**)&mKeyboardState); 

	if( FAILED(hr) )
	{
		// Keyboard lost, zero out keyboard data structure.
		ZeroMemory(mKeyboardState, sizeof(mKeyboardState));

		// Try to acquire for next time we poll.
		hr = mKeyboard->Acquire();
	}

	//===============================================================
	// Poll mouse.

	hr = mMouse->GetDeviceState(sizeof(DIMOUSESTATE2), 
		(void**)&mMouseState); 

	if( FAILED(hr) )
	{
		// Mouse lost, zero out mouse data structure.
		ZeroMemory(&mMouseState, sizeof(mMouseState));

		// Try to acquire for next time we poll.
		hr = mMouse->Acquire(); 
	}
}

bool DInputMgr::keyDown(char key)
{
	return (mKeyboardState[key] & 0x80) != 0;
}

bool DInputMgr::mouseButtonDown(int button)
{
	return (mMouseState.rgbButtons[button] & 0x80) != 0;
}

double DInputMgr::mouseDX()
{
	return (double)mMouseState.lX;
}

double DInputMgr::mouseDY()
{
	return (double)mMouseState.lY;
}

double DInputMgr::mouseDZ()
{
	return (double)mMouseState.lZ;
}

DInputMgr SYS_INPUT;


void Input_Update()
{
//	SYS_INPUT.poll();
//	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);
/*	if( SYS_INPUT.keyDown(DIK_W) )dir += VP1Camera.mLookW;
	if( SYS_INPUT.keyDown(DIK_S) )dir -= VP1Camera.mLookW;
	if( SYS_INPUT.keyDown(DIK_D) )dir += VP1Camera.mRightW;
	if( SYS_INPUT.keyDown(DIK_A) )dir -= VP1Camera.mRightW;*/
	
//	float pitch  = SYS_INPUT.mouseDY() / 150.0f;
//	float yAngle = SYS_INPUT.mouseDX() / 150.0f;
//	VP1Camera.update(dir,yAngle,pitch,0.05f,0);
} 

