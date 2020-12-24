#include "Engine/Setup.h"
#include "Engine/Application/Appclass.h"



LRESULT WINAPI ProcessMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
		
				case VK_F1:
					ACam.CameraChange();
					break;
		
				/*case VK_F2:
					if (RConf.postProcess)RConf.postProcess = false; else RConf.postProcess = true;
					break;
		
				case VK_F3:
					if (RConf.RenderShadows)RConf.RenderShadows = false; else RConf.RenderShadows = true;
					break;
		
				case VK_F4:
					if (RConf.DrawParticles)RConf.DrawParticles = false; else RConf.DrawParticles = true;
					break;
		
				case VK_F5:
					if (PLAYER_FRONT_WEAPON == 0)PLAYER_FRONT_WEAPON = 1; else PLAYER_FRONT_WEAPON = 0;
					break;*/
			}
			break;
		}
		case WM_SETCURSOR:	
			SetCursor(NULL); 
			break;
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


class myGame : public Appclass
{
	private:
		

	public:
		void INIT(HWND hWnd, HINSTANCE hInst);
		void UPDATE_SCENE();
		void RENDER_SCENE();
		void CLEAN();
};


void myGame::INIT(HWND hWnd, HINSTANCE hInst)
{
	dx9Driver.InitDriver(hWnd);
	
	/*SYS_INPUT.InitializeDirectInput(hInst, hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	
	VP1Camera.pos() = D3DXVECTOR3(0.0f, 30.0f, 0.0f);
	VP1Camera.setLens(dx9Driver.VPORT1, 90.0f, dx9Driver.RConf.screenWidth / dx9Driver.RConf.screenHeight, 0.1f, 3000.0f);
	VP1Camera.setSpeed(5.0f);
	VP1Camera.update(D3DXVECTOR3(0.0f, 0.0f, 0.0f), DEG_TO_RAD(0), DEG_TO_RAD(0), 0.05f, 0);
	RM.SetDevice(dx9Driver.VPORT1);


	Init_Physics();
	
	Terr.Init();
	VP1Camera.update(D3DXVECTOR3(0.0f, 0.0f, 0.0f), DEG_TO_RAD(0), DEG_TO_RAD(0), 0.05f, 0);

	dx9Driver.RConf.postProcess = BLOOMEFFECT.Create(dx9Driver.VPORT1);
	dx9Driver.RConf.dynamicSky = SKYDOME.Init_Sky(dx9Driver.VPORT1);

	RM = LoadScene(RM, "System\\Scenes\\Scene.scn", "System//splash.jpg");
	MakeAllObjectsColisional();

	p1.LoadActor("System\\Actor\\Bodies\\MyBody2.plr");
	p1.LoadWheels("System\\Actor\\AutoParts\\w0001.lpm");
	p1.LoadTower("System\\Actor\\AutoParts\\t0001.lpm");
	p1.LoadFront("System\\Actor\\AutoParts\\t0001.lpm");
	p1.SetPosition(V3(10, 7.0, 0));

	ShootVisuals.InitParticles();*/
}

// scene update before drawing
void myGame::UPDATE_SCENE() 
{
	/*ACam.Redraw();

	SYS_INPUT.poll();
	if (SYS_INPUT.keyDown(DIK_SPACE))	p1.Brake();
	if (SYS_INPUT.keyDown(DIK_W))		p1.Gas();
	if (SYS_INPUT.keyDown(DIK_S))		p1.Back();
	if (SYS_INPUT.keyDown(DIK_D))		p1.Turn(2); else
		if (SYS_INPUT.keyDown(DIK_A))		p1.Turn(1); else p1.Turn(0);

	//mouse keys
	if (SYS_INPUT.mouseButtonDown(DIMOUSE_RIGHTBUTTON))
	{
		ACam.AlternativeCamActive = true;
		float yAngle = SYS_INPUT.mouseDX() / 10.0f;
		float pitch = SYS_INPUT.mouseDY() / 10.0f;
		p1.TurnTower(yAngle, pitch);
		if (SYS_INPUT.mouseButtonDown(DIMOUSE_LEFTBUTTON)) { ShootVisuals.Shoot(TOWER_WEAPON, PLAYER_FRONT_WEAPON); }
	}
	else
	{
		ACam.AlternativeCamActive = false;
		if (SYS_INPUT.mouseButtonDown(DIMOUSE_LEFTBUTTON)) { ShootVisuals.Shoot(FRONT_WEAPON, PLAYER_FRONT_WEAPON); }
	}

	pWorld->Update(0.5f);
	ACam.Update();*/
}

// render entire scene
void myGame::RENDER_SCENE() 
{
	dx9Driver.Begin(D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0));
	/*if (dx9Driver.RConf.dynamicSky)SKYDOME.Draw();

	Terr.render_by_depth(TERRAIN_FOV, VP1Camera, FALSE, dx9Driver.RConf.RenderShadows);

	for (int i = ter_sx; i < ter_fx; i++)
		for (int j = ter_sz; j < ter_fz; j++)
			if (VP1Camera.isVisible(LAND_CHUNK[i][j].Box.minPt, LAND_CHUNK[i][j].Box.maxPt))LAND_CHUNK[i][j].RenderCurrentChunk(dx9Driver.RConf.RenderShadows);

	if (INDEP_OBJ.size() > 0)
		if (VP1Camera.isVisible(INDEP_OBJ[0].New.minPt, INDEP_OBJ[0].New.maxPt))
			INDEP_OBJ[0].DrawWithLight(dx9Driver.VPORT1, VP1Camera, RM, TRUE);


	p1.RenderActor();
	ShootVisuals.Draw(PLAYER_FRONT_WEAPON);

	if (dx9Driver.RConf.postProcess)BLOOMEFFECT.PostProcess(dx9Driver.VPORT1);*/
	dx9Driver.End();
}

//clean buffer
void myGame::CLEAN() 
{
	//BLOOMEFFECT.Destroy();
}



INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// Create a pointer to messageProcessing function
	LRESULT(__stdcall * dwp)(HWND, UINT, WPARAM, LPARAM) = &ProcessMessages;
	
	myGame* game = new myGame();

	game->Initialize(hInst, false, ProcessMessages, "GAME ENGINE DEMO");
	game->Run();
	
	return 0;
}
