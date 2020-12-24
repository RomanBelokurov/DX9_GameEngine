#pragma once
#ifndef __APPCLASS__
#define __APPCLASS__

#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>

#include "FrameTimer.h"

class Appclass
{
	private:
		FrameTimer frameTimer;

	public:
		HWND hwnd;

		void Initialize(HINSTANCE hInstance, BOOL ShowCursor, WNDPROC MsgProc, const char* Title);
		void Run();
		
		virtual void INIT(HWND hWnd, HINSTANCE hInst) = 0;
		virtual void UPDATE_SCENE() = 0;
		virtual void RENDER_SCENE() = 0;
		virtual void CLEAN() = 0;
};
#endif