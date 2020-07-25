#ifndef __LGS_SKY__H__
#define __LGS_SKY__H__

#include "LHeader\LIGA_Camera.h"

const char *SKY_SCATER_EFF="\
float4x4  WorldViewProj;\
texture sTexture;\
texture Clouds;\
float Hour;\
float CloudSpeed;\
float Alpha;\
float SunScale;\
float4 SkyColor1 = float4(0.1,0.1,0.1,0.0);\
float4 SkyColor2 = float4(0.1,0.1,0.1,0.0);\
float4 SunDir;\
float4 uv_data: uvScaleOffset = {1.0f,1.0f,0.0f,0.0f};\
\
sampler2D texSampler0 : TEXUNIT0 = sampler_state\
{\
Texture	  = (sTexture);\
MIPFILTER = LINEAR;\
MAGFILTER = LINEAR;\
MINFILTER = LINEAR;\
};\
\
sampler2D texSampler1 : TEXUNIT0 = sampler_state\
{\
Texture	  = (Clouds);\
MIPFILTER = LINEAR;\
MAGFILTER = LINEAR;\
MINFILTER = LINEAR;\
};\
\
struct VS_OUTPUT { \
float4 Pos		: POSITION; \
float4 color	: COLOR0;\
float4 sd		: COLOR1; \
float3 tex0		: TEXCOORD0;\
float2 tex1		: TEXCOORD1;\
float3 Dir		: TEXCOORD2;\
}; \
\
VS_OUTPUT MainVS(float4 Pos : POSITION, float3 Tex : TEXCOORD0) { \
VS_OUTPUT Out; \
Out.tex0  = Tex; \
Out.color=Pos;\
Out.Pos = mul(Pos, WorldViewProj); \
Out.Dir = (Pos);\
Out.sd=normalize(SunDir);\
Out.tex1 = (Pos.xy+uv_data.zw);\
Out.tex1 = Out.tex1 + float2(CloudSpeed, 0.0f);\
return Out; \
} \
float4 PS(VS_OUTPUT inp) : COLOR\
{ \
float4 Color = tex2D( texSampler0, float2( Hour*0.0417, -inp.color.z/2 ) );\
float4 Cloud= tex2D( texSampler1, inp.tex1 );\
float3 dir=normalize(inp.Dir); \
float sdot=(1+dot(dir,inp.sd))/2; \
float4 clr=lerp(SkyColor1,SkyColor2,inp.sd); \
float sun = 0.001/(1.0005-sdot); \
\
return Color+(sun*SunScale)+Cloud/Alpha;\
} \
technique RenderScene \
{ \
pass P0 \
{    \
VertexShader = compile vs_1_0 MainVS();    \
PixelShader = compile ps_2_0 PS(); \
} \
}";

#define SHADOWMAP_SIZE 256
#define SCENE  0
#define SHADOW 1 

class SkyDome
{
protected:
	LPDIRECT3DSURFACE9	m_DepthSurf;
	LPDIRECT3DSURFACE9	m_OrgDepthSurf;
	LPDIRECT3DSURFACE9	m_OrgRenderTarget;
	LPDIRECT3DTEXTURE9	m_Shadowmap;
public:
	LPDIRECT3DDEVICE9 device;
	ID3DXEffect *SkyEffect;
	LPDIRECT3DTEXTURE9 Scatering;
	LPDIRECT3DTEXTURE9 Clouds;
	LPD3DXMESH SKYDOMEMESH;
	float cloudtexpos;
	float scale,hour,rotation,cloudspeed;
	
	void ReloadShadowmap()
	{
		D3DXSaveTextureToFile("D:\\shadow.jpg",D3DXIFF_JPG,m_Shadowmap,NULL);
	}
public:
	ID3DXEffect *m_Effect;
	ID3DXEffect *Lightmap_Effect;	
	D3DXMATRIX View,Proj;
	V3 LightPosition;
	V3 LightColor;
public:

	SkyDome()
	{
		SkyEffect	= NULL;
		LightColor 	= V3(1.0f,1.0f,1.0f);
		device		= NULL;
		Scatering	= NULL;
		Clouds		= NULL;
		SKYDOMEMESH		= NULL;

		m_Effect			= NULL;
		m_Shadowmap			= NULL;
		m_DepthSurf			= NULL;
		m_OrgRenderTarget	= NULL;
		m_OrgDepthSurf		= NULL;
	}
	~SkyDome()
	{
		if(SKYDOMEMESH!=NULL)SKYDOMEMESH->Release();
		if(Scatering!=NULL)Scatering->Release();
		if(Clouds!=NULL)Clouds->Release();

		ReleaseCOM( m_Effect );
		ReleaseCOM( Lightmap_Effect );
		ReleaseCOM( m_Shadowmap );
		ReleaseCOM( m_DepthSurf );
	}

	bool Init_Sky(LPDIRECT3DDEVICE9  g_pd3dDevice);
	bool Init_Shadow();
	void SwitchTargets(DWORD mode = 0);
	void BeginMap();
	void EndMap();
	void SetLight();
	void Update();
	void UpdLightDir(){	D3DXMatrixLookAtLH( &View, &(LightPosition+VP1Camera.pos()), &VP1Camera.pos(), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));}
    void LoadCloudTexture(const char* filename);
	void SetCloudSpeed(int speed){cloudspeed=speed*0.0001f;}

	void SetTime(float Hour)
	{
		float d=Hour/100;
		SkyEffect->SetVector("SunDir",&D3DXVECTOR4(0.0,1.0-d,d,0.0f));
	}

	void SetSunScale(float Scale)
	{
		SkyEffect->SetFloat( "SunScale", Scale*0.1f );
	}

	void SetAlphaClouds(float Alpha)
	{
		SkyEffect->SetFloat( "Alpha", Alpha*0.05f );
	}
	
    void Draw();
};

SkyDome SKYDOME;
bool SHOWSKY=false;

bool SkyDome::Init_Sky(LPDIRECT3DDEVICE9  g_pd3dDevice)
{
	device = g_pd3dDevice;
	if(FAILED(D3DXCreateEffect( device,SKY_SCATER_EFF,strlen(SKY_SCATER_EFF),NULL,NULL,NULL,NULL,&SkyEffect,NULL ))){MessageBox(0, _T("Skyscatering builtin shader error, please contact to developer"), _T("Error"),NULL);return false;}
	SkyEffect->SetTechnique( "RenderScene" );

	if(FAILED(D3DXLoadMeshFromX("System/Effects/unit_dome.un", D3DXMESH_SYSTEMMEM,device, NULL, NULL,NULL, NULL, &SKYDOMEMESH)))
	{ 
		MessageBox(0, _T("Can't load sky module"), _T("Error"),NULL);
		exit(0);
	}
	
	if(FAILED(D3DXCreateTextureFromFileEx(device,"System/Effects/unit_scatering.un",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_FILTER_POINT,NULL,NULL,NULL,&Scatering)))
	{ 
		MessageBox(0, _T("Can't load scaterring module"), _T("Error!"),NULL);
		exit(0);
	}
	
	SkyEffect->SetTexture("sTexture",Scatering);
	SkyEffect->SetTexture("Clouds",NULL);
	scale=10.0f;
	hour=12.0f;
	SetTime(12.0f);
	cloudspeed=0.0005f;
	cloudtexpos=0.0f;
	SetAlphaClouds(45);
	Init_Shadow();
	return true;
}

bool SkyDome::Init_Shadow()
{
	LPD3DXBUFFER pBufferErrors = NULL;
	LPDIRECT3DSURFACE9	OrgDepthSurf=NULL;
	D3DSURFACE_DESC		DepthDesc;
//Lightmaping
	if(FAILED(D3DXCreateEffectFromFile(device,"System/Effects/LightMap.fx",NULL,NULL,0,NULL,&Lightmap_Effect,&pBufferErrors )))
	{
		LPVOID pErrors = pBufferErrors->GetBufferPointer();
		MessageBox(NULL, (const char*)pErrors, ERROR, MB_OK | MB_ICONERROR );
		exit(0);
	}
	Lightmap_Effect->SetTechnique( "LightMapping");
	Lightmap_Effect->SetValue(  "LightAmbientColor", D3DXVECTOR4(0.25f, 0.25f, 0.25f, 1.0f), sizeof(D3DXVECTOR4));
	Lightmap_Effect->SetValue(  "LightDirWorld", V3(1.0f, -1.0f, 1.0f), sizeof(D3DXVECTOR3));

//shadowmaping
	if(FAILED(D3DXCreateEffectFromFile(device,"System/Effects/shadowmap.fx",NULL,NULL,0,NULL,&m_Effect,&pBufferErrors )))
	{
		exit(0);
	}

	if( FAILED(device->CreateTexture( SHADOWMAP_SIZE, SHADOWMAP_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F,D3DPOOL_DEFAULT, &m_Shadowmap, NULL )))
	{
		MessageBox(NULL, "Create texture", "Information",NULL);
		exit(0);
	}

	device->GetDepthStencilSurface( &OrgDepthSurf );
	OrgDepthSurf->GetDesc( &DepthDesc );
	OrgDepthSurf->Release();

	if( FAILED( device->CreateDepthStencilSurface( SHADOWMAP_SIZE, SHADOWMAP_SIZE, DepthDesc.Format,D3DMULTISAMPLE_NONE, 0, TRUE, &m_DepthSurf, NULL )))
	{
		MessageBox(NULL, "Create depthstencil", "Information",NULL);
		exit(0);
	}

	m_Effect->SetTechnique( "ShadowMapping");
	m_Effect->SetTexture( "TexShadow", m_Shadowmap );
	m_Effect->SetValue(  "LightAmbientColor", D3DXVECTOR4(0.25f, 0.25f, 0.25f, 1.0f), sizeof(D3DXVECTOR4));
	m_Effect->SetValue(  "LightDirWorld", V3(1.0f, -1.0f, 1.0f), sizeof(D3DXVECTOR3));
	SetLight();
}

void SkyDome::SetLight()
{
	m_Effect->SetVector("MtrlColor", &D3DXVECTOR4(LightColor, 1.0f)) ;
	m_Effect->SetVector("LPosition", &D3DXVECTOR4(LightPosition, 1.0f)) ;
	Lightmap_Effect->SetVector("MtrlColor", &D3DXVECTOR4(LightColor, 1.0f)) ;
	Lightmap_Effect->SetVector("LPosition", &D3DXVECTOR4(LightPosition, 1.0f)) ;
	D3DXMatrixLookAtLH( &View, &LightPosition, &VP1Camera.pos(), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixOrthoLH( &Proj, 100.0f, 100.0f, 1.0f, 1000.0f );
	m_Effect->CommitChanges();
	Lightmap_Effect->CommitChanges();
}

void SkyDome::Update()
{
	D3DXMATRIX World;
	VPORT1->GetTransform(D3DTS_WORLD, &World);
	m_Effect->SetMatrix( "MatViewerWorld", &World);
	m_Effect->SetMatrix( "MatViewerWVP", &(World * VP1Camera.viewProj()));
	m_Effect->SetMatrix( "MatLightWVP", &(World * View * Proj) );
	m_Effect->CommitChanges();

	Lightmap_Effect->SetMatrix( "MatViewerWorld", &World);
	Lightmap_Effect->SetMatrix( "MatViewerWVP", &(World * VP1Camera.viewProj()));
	Lightmap_Effect->SetMatrix( "MatLightWVP", &(World * View * Proj) );
	Lightmap_Effect->CommitChanges();
}

void SkyDome::LoadCloudTexture(const char* filename)
{
	if(FAILED(D3DXCreateTextureFromFileEx(device,filename,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_FILTER_POINT,NULL,NULL,NULL,&Clouds))){Clouds=NULL;}
	SkyEffect->SetTexture("Clouds",Clouds);
}


void SkyDome::Draw()
{
	device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	device->SetRenderState(D3DRS_ZENABLE, false);

	D3DXMATRIX mTrans;
	D3DXMATRIX World,mm1,mm2,mm3,mrot,mResc;
	D3DXMatrixRotationAxis(&mm1, &V3(1,0,0), DEG_TO_RAD(-90));
	D3DXMatrixRotationAxis(&mm2, &V3(0,1,0), DEG_TO_RAD(rotation));
	D3DXMatrixRotationAxis(&mm3, &V3(0,0,1), DEG_TO_RAD(0));
	mrot=mm3*mm1*mm2;
	D3DXMatrixScaling(&mResc,scale,scale,scale);
	D3DXMatrixTranslation(&mTrans,VP1Camera.pos().x,VP1Camera.pos().y,VP1Camera.pos().z);
	World=mrot*mResc*mTrans;
	device->SetTransform(D3DTS_WORLD, &World);

	SkyEffect->SetFloat( "Hour", hour );
	cloudtexpos=cloudtexpos+cloudspeed;if(cloudtexpos>1.0f)cloudtexpos=0.0f;
	SkyEffect->SetFloat( "CloudSpeed", cloudtexpos );
	SkyEffect->SetMatrix( "WorldViewProj", &(World * VP1Camera.mViewProj) );

	SkyEffect->Begin(0, 0);
	SkyEffect->BeginPass(0);
		SKYDOMEMESH->DrawSubset(0);
	SkyEffect->EndPass();
	SkyEffect->End();

	device->SetTransform(D3DTS_WORLD, &RESET);
	device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	device->SetRenderState(D3DRS_ZENABLE, true);
}

void SkyDome::BeginMap()
{
	SwitchTargets(SHADOW);
	VPORT1->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00FFFFFF, 1.0f, 0L);
	m_Effect->Begin( NULL, 0 );
	m_Effect->BeginPass( 0 );
}

void SkyDome::EndMap()
{
	m_Effect->EndPass();
	m_Effect->End();
	SwitchTargets(SCENE);
}

void SkyDome::SwitchTargets(DWORD mode)
{
	if(mode == SHADOW)
	{
		LPDIRECT3DSURFACE9 ShadowSurface=NULL;
		VPORT1->GetRenderTarget( 0, &m_OrgRenderTarget );
		m_Shadowmap->GetSurfaceLevel( 0, &ShadowSurface );
		VPORT1->SetRenderTarget( 0, ShadowSurface );
		ReleaseCOM( ShadowSurface );
		VPORT1->GetDepthStencilSurface( &m_OrgDepthSurf );
		VPORT1->SetDepthStencilSurface( m_DepthSurf );
	}
	 else
	{
		VPORT1->SetDepthStencilSurface( m_OrgDepthSurf );
		ReleaseCOM( m_OrgDepthSurf );
		VPORT1->SetRenderTarget( 0, m_OrgRenderTarget );
		ReleaseCOM( m_OrgRenderTarget );
	}
}

#endif __LGS_SKY__H__

