
D3DXMATRIX 			RESET;
 
LPDIRECT3DDEVICE9 VPORT1;
D3DPRESENT_PARAMETERS	g_D3Dpp;

V3 GLOBAL_FOG_COLOR;
int GLOBAL_FOG_MIN,GLOBAL_FOG_MAX;
//FLOAT CFG[3];

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
		screenWidth=1920;
		screenHeight=1080;
		refreshRate=75;
		dynamicSky=false;
		postProcess=false;
		RenderShadows=true;
		DrawParticles=true;
	}
};

RenderConfig RConf;

bool LoadInitConfig(const char* cfgfile)
{
	FILE *fp;
	char buf[25]="",str[25]="";
	int j=0;
	
	fp=fopen(cfgfile,"r");
	if(fp==NULL){printf("Render configuration file opening error\n");return FALSE;}
	else
	{	
		while(!feof(fp))
		{
			fgets(buf,25,fp);
						
			if(strstr(buf,"Width")!=NULL)
			{for(int i=0;i<24;i++)
			{	if(isalpha(buf[i])==0){str[j]=buf[i];j++;}
			if(buf[i]=='\n')break;
			}
			RConf.screenWidth=atof(str);j=0;
			}

			if(strstr(buf,"Height")!=NULL)
			{for(int i=0;i<24;i++)
			{	if(isalpha(buf[i])==0){str[j]=buf[i];j++;}
			if(buf[i]=='\n')break;
			}
			RConf.screenHeight=atof(str);j=0;
			}			
			if(strstr(buf,"RefreshHZ")!=NULL)
			{for(int i=0;i<24;i++)
			{	if(isalpha(buf[i])==0){str[j]=buf[i];j++;}
			if(buf[i]=='\n')break;
			}
			RConf.refreshRate=atof(str);j=0;
			}
		}
	}
	if(fp!=NULL)fclose(fp);
	return TRUE;
}


void EnableAlpha(LPDIRECT3DDEVICE9 g_pd3dDevice);
void DisableAlpha(LPDIRECT3DDEVICE9 g_pd3dDevice);

LPDIRECT3DDEVICE9 LE_Init_Engine(HWND hWnd,HINSTANCE hInst,const char *ConfigFile)
{
	LPDIRECT3D9				g_pD3D;

	LoadInitConfig(ConfigFile);

	if(!(g_pD3D=Direct3DCreate9( D3D_SDK_VERSION )))
	{
		printf(0,"Trouble in Direct3DCreate9()","Error",NULL);
		exit(0);
	}
	
	ZeroMemory( &g_D3Dpp, sizeof(g_D3Dpp) );

	g_D3Dpp.BackBufferWidth            = RConf.screenWidth;
	g_D3Dpp.BackBufferHeight           = RConf.screenHeight;
	
	D3DDISPLAYMODE d3ddm;
	memset(&d3ddm,0,sizeof(D3DDISPLAYMODE));
	g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm);
	g_D3Dpp.BackBufferFormat			= d3ddm.Format;

	D3DFORMAT dispFormat[]={D3DFMT_D32,D3DFMT_D24S8,D3DFMT_D24X4S4,D3DFMT_D24X8,D3DFMT_D16};
	for(size_t j=0;j<sizeof(dispFormat)/sizeof(dispFormat[0]);++j)
	{
		if(FAILED(g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,dispFormat[j])))continue;
		g_D3Dpp.AutoDepthStencilFormat=dispFormat[j];
		break;
	}
	
	g_D3Dpp.Windowed					= FALSE;
	g_D3Dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	g_D3Dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;//RConf.refreshRate;
	g_D3Dpp.hDeviceWindow				= hWnd;
	g_D3Dpp.EnableAutoDepthStencil		= TRUE; 
	g_D3Dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	if(FAILED(g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_D3Dpp, &VPORT1 )))
	{
		if ( FAILED( g_pD3D->CreateDevice(  D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING , &g_D3Dpp, &VPORT1 )))		
		{
			ReleaseCOM( g_pD3D );
			exit(0);
		}
	}

	VPORT1->SetRenderState (D3DRS_ZENABLE, D3DZB_TRUE);
	VPORT1->SetRenderState( D3DRS_LIGHTING, FALSE );
	VPORT1->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	VPORT1->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	VPORT1->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	VPORT1->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	VPORT1->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	VPORT1->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	VPORT1->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	
	D3DXMatrixIdentity(&RESET);
	EnableAlpha(VPORT1);
	ReleaseCOM(g_pD3D);
}

void Begin(DWORD param,D3DCOLOR color)
{
    VPORT1->Clear( 0, NULL, param, color, 1.0f, 0 );
    VPORT1->BeginScene();
}


void End(){
	VPORT1->EndScene();
    VPORT1->Present( NULL, NULL, NULL, NULL );
}

void EnableAlpha(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0xCC );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void DisableAlpha(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0xCC );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void SetFog(LPDIRECT3DDEVICE9 g_pd3dDevice,D3DCOLOR color,float fFogStart,float fFogEnd)
{
	g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_LINEAR);
	g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR,color);
	g_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*) (&fFogStart)));
	g_pd3dDevice->SetRenderState(D3DRS_FOGEND, *((DWORD*) (&fFogEnd)));
}
 

void DrawLine(LPDIRECT3DDEVICE9 g_pd3dDevice, V3 LineStart,V3 LineEnd,D3DCOLOR color)
{
	struct{V3 coord;D3DCOLOR color;}verticies[2] = {LineStart, color,LineEnd, color};
	g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, verticies, sizeof(verticies[0]));
}

void DrawGrid(LPDIRECT3DDEVICE9 g_pd3dDevice,D3DCOLOR color)
{
	D3DXMATRIX World,mm1,mm2,mm3,mrot,mResc,mTrans;
	D3DXMatrixTranslation(&mTrans,0,0,0);
	D3DXMatrixRotationX(&mm1,DEG_TO_RAD(0));
	D3DXMatrixRotationY(&mm2,DEG_TO_RAD(0));
	D3DXMatrixRotationZ(&mm3,DEG_TO_RAD(0));
	mrot=mm1*mm2*mm3;
	D3DXMatrixScaling(&mResc,1,1,1);
	World=mrot*mResc*mTrans;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &World);
	for(int i=-25;i<=25;i++)
	{
		DrawLine(g_pd3dDevice,V3(-500.0, 0.0, 20*i),V3(500.0, 0.0, 20*i),color);
		DrawLine(g_pd3dDevice,V3(20*i, 0.0, -500.0),V3(20*i, 0.0, 500.0),color);
	}
} 

//SPHERE TRANS
V3 SphereTrans(V3 RotationPoint,V3 Point,V3 Angle)
{
	V3 Vect;
	D3DXMATRIX m1,m2,m3,m;
	Vect=Point-RotationPoint;

	D3DXMatrixRotationX( &m1, D3DXToRadian( Angle.x ) );
	D3DXMatrixRotationY( &m2, D3DXToRadian( Angle.y ) );
	D3DXMatrixRotationZ( &m3, D3DXToRadian( Angle.z ) );
	m=m3*m1*m2;
	D3DXVec3TransformCoord(&Vect, &Vect, &m);
	Point=Vect+RotationPoint;
	return Point;
}


 