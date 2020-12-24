#include "Appclass.h"

void Appclass::Initialize(HINSTANCE hInstance, BOOL ShowCursor, WNDPROC MsgProc, const char* Title)
{
	WNDCLASSEX wc;
	HWND hWnd;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = (WNDPROC)MsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	if (ShowCursor)wc.hCursor = LoadCursor(NULL, IDC_ARROW); else wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = 0;
	wc.lpszClassName = "WINXCLASS";
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);
	if (FAILED(hWnd = CreateWindow("WINXCLASS", Title, WS_OVERLAPPEDWINDOW, 100, 100, 640, 480, NULL, NULL, hInstance, NULL)))
	{
		MessageBox(0, _T("Unable to create window"), _T("Failed"), NULL);
		exit(0);
	}

	// Initialize in game timer
	frameTimer.Init();
	frameTimer.Item(0, 10);

	INIT(hWnd, hInstance);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
}

void Appclass::Run()
{
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			frameTimer.Refresh();
			//ACam.Redraw();
			if (frameTimer.Timer(0))
			{
				UPDATE_SCENE();
				RENDER_SCENE();
			}

		}
	}
}
