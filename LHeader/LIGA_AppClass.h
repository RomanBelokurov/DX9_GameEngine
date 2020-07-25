LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					{
						PostQuitMessage(0);
					}
				case VK_F1:
					{
						ACam.CameraChange();
						break; 
					}
				case VK_F2:
					{
						if(RConf.postProcess)RConf.postProcess=false;else RConf.postProcess=true;
						break;
					}
				case VK_F3:
					{
						if(RConf.RenderShadows)RConf.RenderShadows=false;else RConf.RenderShadows=true;
						break;
					}
				case VK_F4:
					{
						if(RConf.DrawParticles)RConf.DrawParticles=false;else RConf.DrawParticles=true;
						break;
					}
				case VK_F5:
					{
						if(PLAYER_FRONT_WEAPON==0)PLAYER_FRONT_WEAPON=1;else PLAYER_FRONT_WEAPON=0;
						break;
					}					
			}
		}
	case WM_SETCURSOR:	SetCursor (NULL);break;
		break;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}



class LGS_APP
{
public:
	HWND hwnd;

	~LGS_APP();
	void Initialize(HINSTANCE hInstance,BOOL ShowCursor,const char*Title);
	void Run();
	
	void INIT(HWND hWnd,HINSTANCE hInst);
	void UPDATE_SCENE();
	void RENDER_SCENE();
	void CLEAN();
	void ThreadTiming();
};

LGS_APP::~LGS_APP()
{
	ReleaseCOM(VPORT1);
	CLEAN();
}
void LGS_APP::Initialize(HINSTANCE hInstance,BOOL ShowCursor,const char*Title)
{
	WNDCLASSEX wc;
	HWND hWnd;
	wc.cbSize = sizeof(WNDCLASSEX); 
	wc.style			= CS_CLASSDC;
	wc.lpfnWndProc		= (WNDPROC)MsgProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	if(ShowCursor)wc.hCursor=LoadCursor(NULL, IDC_ARROW);else wc.hCursor=NULL;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= "WINXCLASS";
	wc.hIconSm			= NULL;

	RegisterClassEx(&wc);
	if(FAILED(hWnd = CreateWindow("WINXCLASS", Title, WS_OVERLAPPEDWINDOW,100, 100, 640,480, NULL, NULL, hInstance, NULL)))
	{
		MessageBox(0, _T("Unable to create window"), _T("Failed"),NULL);
		exit(0);
	}

	INIT(hWnd,hInstance);

	ShowWindow( hWnd, SW_SHOWDEFAULT );
	UpdateWindow( hWnd );
}


void LGS_APP::Run()
{
	MSG msg = {};
	
	while( msg.message != WM_QUIT )
	{       
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			heTimer.Refresh();
			ACam.Redraw();
			if( heTimer.Timer( 0 ) )
			{
				UPDATE_SCENE();
				RENDER_SCENE();				
			}

		}
	}
}



void LGS_APP::INIT(HWND hWnd,HINSTANCE hInst)
{
	LE_Init_Engine(hWnd,hInst,"render.cfg");
	SYS_INPUT.InitializeDirectInput(hInst, hWnd, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND,DISCL_EXCLUSIVE|DISCL_FOREGROUND);
	heTimer.Init();
	heTimer.Item( 0, 10 );
	VP1Camera.pos()=D3DXVECTOR3(0.0f,30.0f,0.0f);
	VP1Camera.setLens(VPORT1,90.0f,RConf.screenWidth/RConf.screenHeight,0.1f,3000.0f);
	VP1Camera.setSpeed(5.0f);
	VP1Camera.update(D3DXVECTOR3(0.0f,0.0f,0.0f),DEG_TO_RAD(0),DEG_TO_RAD(0),0.05f,0);
	RM.SetDevice(VPORT1);	
	Init_Physics();
	Terr.Init();
	VP1Camera.update(D3DXVECTOR3(0.0f,0.0f,0.0f),DEG_TO_RAD(0),DEG_TO_RAD(0),0.05f,0);	
	
	RConf.postProcess=BLOOMEFFECT.Create(VPORT1);
	RConf.dynamicSky=SKYDOME.Init_Sky(VPORT1);
	
	RM=LoadScene(RM, "System\\Scenes\\Scene.scn","System//splash.jpg");
	MakeAllObjectsColisional();

	p1.LoadActor("System\\Actor\\Bodies\\MyBody2.plr");
	p1.LoadWheels("System\\Actor\\AutoParts\\w0001.lpm");
	p1.LoadTower("System\\Actor\\AutoParts\\t0001.lpm");
	p1.LoadFront("System\\Actor\\AutoParts\\t0001.lpm");
	p1.SetPosition(V3(10, 7.0, 0));

	ShootVisuals.InitParticles();
}

V3 qPos=V3(0,0,0);
void UpdateShadow()
{
	if(qPos!=V3(ter_sx,ter_sz,0))
	{
		SKYDOME.UpdLightDir();
		SKYDOME.BeginMap();
			for(int i=ter_sx;i<ter_fx;i++)for(int j=ter_sz;j<ter_fz;j++)LAND_CHUNK[i][j].RenderCurrentChunkFS();
			for(int i=0;i<INDEP_OBJ.size();i++)INDEP_OBJ[i].DrawForShadowMap(VPORT1,VP1Camera,RM);
		SKYDOME.EndMap();
		qPos=V3(ter_sx,ter_sz,0);
	}
}

void LGS_APP::RENDER_SCENE()
{	
	Begin(D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
		if(RConf.RenderShadows)UpdateShadow();
		if(RConf.dynamicSky)SKYDOME.Draw();

		Terr.render_by_depth(TERRAIN_FOV,VP1Camera,FALSE,RConf.RenderShadows);
		
		for(int i=ter_sx;i<ter_fx;i++)
			for(int j=ter_sz;j<ter_fz;j++)
			if(VP1Camera.isVisible(LAND_CHUNK[i][j].Box.minPt,LAND_CHUNK[i][j].Box.maxPt))LAND_CHUNK[i][j].RenderCurrentChunk(RConf.RenderShadows);
		
		if(INDEP_OBJ.size()>0)
			if(VP1Camera.isVisible(INDEP_OBJ[0].New.minPt,INDEP_OBJ[0].New.maxPt))
				INDEP_OBJ[0].DrawWithLight(VPORT1,VP1Camera,RM,TRUE);
		
		
		//for(int i=0;i<TRIGGER_VECTOR.size();i++)TRIGGER_VECTOR[i].Draw();
		p1.RenderActor();		
		ShootVisuals.Draw(PLAYER_FRONT_WEAPON);

		if(RConf.postProcess)BLOOMEFFECT.PostProcess(VPORT1);
	End();
}


void LGS_APP::UPDATE_SCENE()
{
	SYS_INPUT.poll();
	if( SYS_INPUT.keyDown(DIK_SPACE))	p1.Brake();
	if( SYS_INPUT.keyDown(DIK_W))		p1.Gas();
	if( SYS_INPUT.keyDown(DIK_S))		p1.Back();
	if( SYS_INPUT.keyDown(DIK_D))		p1.Turn(2);else
	if( SYS_INPUT.keyDown(DIK_A))		p1.Turn(1); else p1.Turn(0);
	
	//mouse keys
	if(SYS_INPUT.mouseButtonDown(DIMOUSE_RIGHTBUTTON))
	{
		ACam.AlternativeCamActive=true;
		float yAngle = SYS_INPUT.mouseDX() / 10.0f;
		float pitch  = SYS_INPUT.mouseDY() / 10.0f;
		p1.TurnTower(yAngle,pitch);
		if(SYS_INPUT.mouseButtonDown(DIMOUSE_LEFTBUTTON)){ShootVisuals.Shoot(TOWER_WEAPON,PLAYER_FRONT_WEAPON);}
	}else 
	{
		ACam.AlternativeCamActive=false;
		if(SYS_INPUT.mouseButtonDown(DIMOUSE_LEFTBUTTON)){ShootVisuals.Shoot(FRONT_WEAPON,PLAYER_FRONT_WEAPON);}
	}
	
	pWorld->Update(0.5f);
	ACam.Update();	
}


void LGS_APP::CLEAN()
{
	BLOOMEFFECT.Destroy();
}

LGS_APP* App;

