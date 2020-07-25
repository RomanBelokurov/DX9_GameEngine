
//*******************************************************
//In this header we Create Terrrain
//*******************************************************

#ifndef __LGS_TERRAIN__H__
#define __LGS_TERRAIN__H__

# include <dxfile.h>
#include "LHeader\LIGA_QuadTree.h"

# define VERTEXFORMAT		(D3DFVF_XYZ | D3DFVF_TEX1 )


BOOL TERRAIN_LOADED_ALLREADY=FALSE;

const char *TERRAIN_EFF="\
float4x4 gViewProj;\
float4   LightColor;\
texture  gTex0;\
texture  gTex1;\
texture  gTex2;\
texture  gBlendMap;\
\
static float gTexScale = 500.0f;\
\
sampler Tex0S = sampler_state\
{\
	Texture = <gTex0>;\
	MinFilter = linear;\
	MagFilter = Linear;\
	MipFilter = Linear;\
};\
\
sampler Tex1S = sampler_state\
{\
	Texture = <gTex1>;\
	MinFilter = linear;\
	MagFilter = Linear;\
	MipFilter = Linear;\
};\
\
sampler Tex2S = sampler_state\
{\
	Texture = <gTex2>;\
	MinFilter = linear;\
	MagFilter = Linear;\
	MipFilter = Linear;\
};\
\
sampler BlendMapS = sampler_state\
{\
	Texture = <gBlendMap>;\
	MinFilter = LINEAR;\
	MagFilter = LINEAR;\
	MipFilter = LINEAR;\
	AddressU  = WRAP;\
	AddressV  = WRAP;\
};\
\
struct OutputVS\
{\
	float4 posH         : POSITION0;\
	float2 tiledTexC    : TEXCOORD0;\
	float2 nonTiledTexC : TEXCOORD1;\
};\
\
OutputVS TerrainVS(float3 posW : POSITION0,float3 normalW : NORMAL0,float2 tex0: TEXCOORD0)\
{\
	OutputVS outVS = (OutputVS)0;\
	outVS.posH = mul(float4(posW, 1.0f), gViewProj);\
	outVS.tiledTexC    = tex0 * gTexScale;\
	outVS.nonTiledTexC = tex0;\
	return outVS;\
}\
\
float4 TerrainPS(float2 tiledTexC : TEXCOORD0,float2 nonTiledTexC : TEXCOORD1,float shade : TEXCOORD2) : COLOR\
{\
	float3 c0 = tex2D(Tex0S, tiledTexC).rgb;\
	float3 c1 = tex2D(Tex1S, tiledTexC).rgb;\
	float3 c2 = tex2D(Tex2S, tiledTexC).rgb;\
	float3 B = tex2D(BlendMapS, nonTiledTexC).rgb;\
	float totalInverse = 1.0f / (B.r + B.g + B.b);\
	c0 *= B.r * totalInverse;\
	c1 *= B.g * totalInverse;\
	c2 *= B.b * totalInverse;\
	float3 final = (c0 + c1 + c2)*LightColor;\
	return float4(final, 1.0f);\
}\
\
technique TerrainTech\
{\
	pass P0\
	{\
		vertexShader = compile vs_2_0 TerrainVS();\
		pixelShader  = compile ps_2_0 TerrainPS();\
	}\
}";


int grauwert( D3DCOLOR c)
{
	DWORD r, g, b;
	int grau;
	r = (c >> 16) & 0xff;
	g = (c >> 8) & 0xff;
	b = c & 0xff;
	grau = (int)(0.299f * r + 0.587f * g + 0.114f *b);
	return grau;
}

LPD3DXMESH Create_Empty_Mesh(LPDIRECT3DDEVICE9 device, int xgrid, int zgrid, int Step)
{
	LPD3DXMESH mesh;

	DWORD *pi;
	int x, z, i;
	vertex *mv;

	if(FAILED(D3DXCreateMeshFVF( 2*xgrid*zgrid, (xgrid+1)*(zgrid+1), D3DXMESH_MANAGED|D3DXMESH_32BIT, VERTEXFORMAT, device, &mesh))){MessageBox(0,"Mesh creation failed","",NULL);exit(0);}
	mesh->LockIndexBuffer( D3DLOCK_NOSYSLOCK, (void **)&pi);
	for( z = 0, i = 0; z < zgrid; z++)
		{
		for( x = 0; x < xgrid; x++, i+=6)
			{
			pi[i] = z*(xgrid+1) + x;
			pi[i+1] = (z+1)*(xgrid+1) + x;
			pi[i+2] = (z+1)*(xgrid+1) + x + 1;

			pi[i+3] = z*(xgrid+1) + x;
			pi[i+4] = (z+1)*(xgrid+1) + x + 1;
			pi[i+5] = z*(xgrid+1) + x + 1;
			}
		}
	mesh->UnlockIndexBuffer();

	mesh->LockVertexBuffer( D3DLOCK_NOSYSLOCK, (void **)&mv);
	for( z = 0, i = 0; z <= zgrid; z++)
		{
		for( x = 0; x <= xgrid; x++, i++)
			{
				mv[i].pos.x = Step*x;
				mv[i].pos.y = 0;
				mv[i].pos.z = Step*z;
				mv[i].u = ((float)x)/xgrid;
				mv[i].v = 1.0f - ((float)z)/zgrid;
			}
		}
	mesh->UnlockVertexBuffer();
	return mesh;
}

bool DetailTexExists=FALSE;

class terragen
{
public:
	LPDIRECT3DDEVICE9 device;
	ID3DXEffect *TerrainEffect;
	LPDIRECT3DTEXTURE9 heightmap;
	LPDIRECT3DTEXTURE9 blendmap;
	LPDIRECT3DTEXTURE9 layer1map;
	LPDIRECT3DTEXTURE9 layer2map;
	LPDIRECT3DTEXTURE9 layer3map;

public:
	void Init()
	{
		LPD3DXBUFFER pBufferErrors = NULL;
		if(FAILED(D3DXCreateEffect( VPORT1,TERRAIN_EFF,strlen(TERRAIN_EFF),NULL,NULL,NULL,NULL,&TerrainEffect,&pBufferErrors )))
		{
			LPVOID pErrors = pBufferErrors->GetBufferPointer();
			MessageBox(NULL, (const char*)pErrors, ERROR, MB_OK | MB_ICONERROR );
			exit(0);
		}
		TerrainEffect->SetTechnique( "TerrainTech" );
		TerrainEffect->SetVector("LightColor", &D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f));
    }

	void BildTerrain(LPDIRECT3DDEVICE9 dev, const char *hmapfile,int xgrid,int zgrid,int scale,int Step);

	void LoadTextures(const char* BlendMap, const char* Layer1, const char* Layer2, const char* Layer3)
	{
		if( blendmap)blendmap->Release();blendmap=NULL;
		if( layer1map)layer1map->Release();layer1map=NULL;
		if( layer2map)layer2map->Release();layer2map=NULL;
		if( layer3map)layer3map->Release();layer3map=NULL;
		D3DXCreateTextureFromFileEx(device,BlendMap,512,512,3,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,NULL,NULL,NULL,&blendmap);
		D3DXCreateTextureFromFileEx(device,Layer1,512,512,3,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,NULL,NULL,NULL,&layer1map);
		D3DXCreateTextureFromFileEx(device,Layer2,512,512,3,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,NULL,NULL,NULL,&layer2map);
		D3DXCreateTextureFromFileEx(device,Layer3,512,512,3,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,NULL,NULL,NULL,&layer3map);

		TerrainEffect->SetTexture("gTex0", layer1map);
		TerrainEffect->SetTexture("gTex1", layer2map);
		TerrainEffect->SetTexture("gTex2", layer3map);
		TerrainEffect->SetTexture("gBlendMap", blendmap);
	}

	void render(DXCamera CAM,BOOL render_quads=FALSE);
	void render_by_depth(int Depth,DXCamera CAM,BOOL render_quads=FALSE,BOOL DrawShadows=false);
	void Update(){TerrainEffect->SetVector("LightColor", &D3DXVECTOR4(SKYDOME.LightColor, 1.0f));}
	~terragen()
	{
		for(int i=0;i<TERRAIN_SCALE;i++)
			for(int j=0;j<TERRAIN_SCALE;j++)if(LAND_CHUNK[i][j].Land_Chunk)LAND_CHUNK[i][j].Land_Chunk->Release();
		if( heightmap)heightmap->Release();
		if( blendmap)blendmap->Release();
		if( layer1map)layer1map->Release();
		if( layer2map)layer2map->Release();
		if( layer3map)layer3map->Release(); 
	}
};

terragen Terr;

void terragen::BildTerrain(LPDIRECT3DDEVICE9 dev, const char *hmapfile,int xgrid,int zgrid,int scale,int Step)
{
	device=dev;
	int CHUNKS_COUNT=scale*scale;
	TERRAIN_SCALE=scale;
	for(int i=0;i<scale;i++)
		for(int j=0;j<scale;j++)if(LAND_CHUNK[i][j].Land_Chunk)LAND_CHUNK[i][j].Land_Chunk->Release();

	for(int i=0;i<scale;i++)
		for(int j=0;j<scale;j++)LAND_CHUNK[i][j].Land_Chunk=Create_Empty_Mesh(device,xgrid/scale,zgrid/scale,Step);


	UINT width, height;
	D3DSURFACE_DESC dsc;
	if( heightmap){heightmap->Release();heightmap=NULL;}
	D3DXCreateTextureFromFile( device, hmapfile, &heightmap);

	heightmap->GetLevelDesc( 0, &dsc);
	width = dsc.Width;
	int CenterX=width/2*Step;
	height = dsc.Height;
	int CenterZ=height/2*Step;

	D3DLOCKED_RECT lr;
	heightmap->LockRect( 0, &lr, NULL, D3DLOCK_READONLY);

	//**********************************
	vertex *mv;
	int x, z, i,c;
	int zeile, spalte;
	int gw;

for(int s=0;s<scale;s++)
for(int c=0;c<scale;c++)
{
	LAND_CHUNK[c][s].Land_Chunk->LockVertexBuffer( D3DLOCK_NOSYSLOCK, (void **)&mv);
	for( z = s*(zgrid/scale), i = 0; z <= (s+1)*(zgrid/scale); z++)
		for( x = c*(xgrid/scale); x <= (c+1)*(xgrid/scale); x++, i++)
		{
			zeile = ((height-1)*(zgrid-z))/zgrid;
			spalte = ((width-1)*x)/xgrid;
			gw = grauwert( *(((D3DCOLOR *)lr.pBits) + width*zeile + spalte));

			mv[i].pos.y = 0.1*gw;
			mv[i].pos.x = Step*x-CenterX;
			mv[i].pos.z = Step*z-CenterZ;

			mv[i].u = ((float)x)/xgrid;
			mv[i].v = 1.0f - ((float)z)/zgrid;
		}
	LAND_CHUNK[c][s].Land_Chunk->UnlockVertexBuffer();
}
	//**********************************

	heightmap->UnlockRect( 0);
	if( heightmap){heightmap->Release();heightmap=NULL;}

	for(int i=0;i<TERRAIN_SCALE;i++)
		for(int j=0;j<TERRAIN_SCALE;j++)LAND_CHUNK[i][j].ComputeBoundingBox(device);
	TERRAIN_LOADED_ALLREADY=TRUE;
}

V2 GetTextureParams(LPDIRECT3DDEVICE9 dev,char* Filename)
{
	LPDIRECT3DTEXTURE9 heightmap;
	V2 tmp;
	D3DSURFACE_DESC dsc;

	D3DXCreateTextureFromFile( dev, Filename, &heightmap);
	heightmap->GetLevelDesc( 0, &dsc);
	tmp.x = dsc.Width;
	tmp.y = dsc.Height;

	if( heightmap)heightmap->Release();
	return tmp;
}

void terragen::render(DXCamera CAM,BOOL render_quads)
{
if(TERRAIN_LOADED_ALLREADY){

	TerrainEffect->SetMatrix("gViewProj", &CAM.mViewProj);
	TerrainEffect->Begin( NULL, 0 );
	TerrainEffect->BeginPass( 0 );
	for(int i=0;i<TERRAIN_SCALE;i++)
		for(int j=0;j<TERRAIN_SCALE;j++)
		{
			if(CAM.isVisible(LAND_CHUNK[i][j].ChunkAABB.minPt,LAND_CHUNK[i][j].ChunkAABB.maxPt))LAND_CHUNK[i][j].Land_Chunk->DrawSubset( 0);
		}
	TerrainEffect->EndPass();
	TerrainEffect->End();

	if(render_quads)
	{
		for(int i=0;i<TERRAIN_SCALE;i++)
			for(int j=0;j<TERRAIN_SCALE;j++)
			{
				DrawBB(device,LAND_CHUNK[i][j].ChunkAABB.minPt,LAND_CHUNK[i][j].ChunkAABB.maxPt,D3DCOLOR_XRGB(255,0,0));
				DrawBB(device,LAND_CHUNK[i][j].Box.minPt,LAND_CHUNK[i][j].Box.maxPt,D3DCOLOR_XRGB(0,255,0));
			}
	}
 }
}


int ter_sx,ter_sz,ter_fx,ter_fz;

void FIND_SELF_IN_TERRAIN(DXCamera CAM,int Depth)
{
if(TERRAIN_LOADED_ALLREADY){
	V3 POS=V3(0,0,0);
	for(int i=0;i<TERRAIN_SCALE;i++)
		for(int j=0;j<TERRAIN_SCALE;j++)
		{
			if(LAND_CHUNK[i][j].ChunkAABB.PointInColon(&CAM.pos()))
			{
				POS.x=i;POS.z=j;
				ter_sx=POS.x-Depth;	if(ter_sx<=0)ter_sx=0;
				ter_sz=POS.z-Depth;	if(ter_sz<=0)ter_sz=0;

				ter_fx=POS.x+Depth+1;if(ter_fx>=TERRAIN_SCALE)ter_fx=TERRAIN_SCALE;
				ter_fz=POS.z+Depth+1;if(ter_fz>=TERRAIN_SCALE)ter_fz=TERRAIN_SCALE;
			}
		}
 }
}


void FIND_SELF_IN_LOCAL_TERRAIN(DXCamera CAM,int Depth)
{
if(TERRAIN_LOADED_ALLREADY){
	V3 POS=V3(0,0,0);
	bool PlaceFoundInLocal=FALSE;

	for(int i=ter_sx;i<ter_fx;i++)
		for(int j=ter_sz;j<ter_fz;j++)
		{
			if(LAND_CHUNK[i][j].ChunkAABB.PointInColon(&CAM.pos()))
			{
				PlaceFoundInLocal=TRUE;
				POS.x=i;POS.z=j;
				ter_sx=POS.x-Depth;	if(ter_sx<=0)ter_sx=0;
				ter_sz=POS.z-Depth;	if(ter_sz<=0)ter_sz=0;

				ter_fx=POS.x+Depth+1;if(ter_fx>=TERRAIN_SCALE)ter_fx=TERRAIN_SCALE;
				ter_fz=POS.z+Depth+1;if(ter_fz>=TERRAIN_SCALE)ter_fz=TERRAIN_SCALE;
			}
		}
	if(!PlaceFoundInLocal)FIND_SELF_IN_TERRAIN(CAM,Depth);
 }
}


void terragen::render_by_depth(int Depth,DXCamera CAM,BOOL render_quads,BOOL DrawShadows)
{
if(TERRAIN_LOADED_ALLREADY){
	FIND_SELF_IN_LOCAL_TERRAIN(CAM,Depth);

	if(!DrawShadows)
	{
		TerrainEffect->SetMatrix("gViewProj", &CAM.mViewProj);
		TerrainEffect->Begin( NULL, 0 );
		TerrainEffect->BeginPass( 0 );
		for(int i=ter_sx;i<ter_fx;i++)
			for(int j=ter_sz;j<ter_fz;j++)
			{
				if(CAM.isVisible(LAND_CHUNK[i][j].ChunkAABB.minPt,LAND_CHUNK[i][j].ChunkAABB.maxPt))LAND_CHUNK[i][j].Land_Chunk->DrawSubset( 0);
			}
		TerrainEffect->EndPass();
		TerrainEffect->End();
	}else
	{
		SKYDOME.Update();
		SKYDOME.m_Effect->SetTexture("TexScene", layer1map);
		SKYDOME.m_Effect->SetTexture("TexSceneDet", layer2map);
		SKYDOME.m_Effect->SetTexture("Layer4", layer3map);
		SKYDOME.m_Effect->SetTexture("Blend", blendmap);
		SKYDOME.m_Effect->CommitChanges();
		SKYDOME.m_Effect->Begin( NULL, 0 );
		SKYDOME.m_Effect->BeginPass( 2 );
		for(int i=ter_sx;i<ter_fx;i++)
			for(int j=ter_sz;j<ter_fz;j++)
			{
				if(CAM.isVisible(LAND_CHUNK[i][j].ChunkAABB.minPt,LAND_CHUNK[i][j].ChunkAABB.maxPt))LAND_CHUNK[i][j].Land_Chunk->DrawSubset( 0);
			}
		SKYDOME.m_Effect->EndPass();
		SKYDOME.m_Effect->End();
	}

	if(render_quads)
	{
		for(int i=ter_sx;i<ter_fx;i++)
			for(int j=ter_sz;j<ter_fz;j++)
			{
				DrawBB(device,LAND_CHUNK[i][j].ChunkAABB.minPt,LAND_CHUNK[i][j].ChunkAABB.maxPt,D3DCOLOR_XRGB(255,0,0));
				DrawBB(device,LAND_CHUNK[i][j].Box.minPt,LAND_CHUNK[i][j].Box.maxPt,D3DCOLOR_XRGB(0,255,0));
			}
	}
 }
}


void CreateTerrainPhysics(LPDIRECT3DDEVICE9 device, const char *hmapfile, int xgrid, int zgrid, int Step)
{	
	LPD3DXMESH mesh;
	LPDIRECT3DTEXTURE9 hm;
	D3DSURFACE_DESC dsc;
	D3DXCreateTextureFromFile( device, hmapfile, &hm);

	hm->GetLevelDesc( 0, &dsc);
	
	UINT width = dsc.Width;		int CenterX=width/2*Step;
	UINT height = dsc.Height;	int CenterZ=height/2*Step;

	D3DLOCKED_RECT lr;
	hm->LockRect( 0, &lr, NULL, D3DLOCK_READONLY);

	DWORD *pi;
	int x, z, i;
	vertex *mv;
	int zeile, spalte;
	int gw;

	if(FAILED(D3DXCreateMeshFVF( 2*xgrid*zgrid, (xgrid+1)*(zgrid+1), D3DXMESH_MANAGED|D3DXMESH_32BIT, VERTEXFORMAT, device, &mesh))){MessageBox(0,"Mesh creation failed","",NULL);exit(0);}
	mesh->LockIndexBuffer( D3DLOCK_NOSYSLOCK, (void **)&pi);
	
	for( z = 0, i = 0; z < zgrid; z++)
		for( x = 0; x < xgrid; x++, i+=6)
		{
			pi[i] = z*(xgrid+1) + x;
			pi[i+1] = (z+1)*(xgrid+1) + x;
			pi[i+2] = (z+1)*(xgrid+1) + x + 1;

			pi[i+3] = z*(xgrid+1) + x;
			pi[i+4] = (z+1)*(xgrid+1) + x + 1;
			pi[i+5] = z*(xgrid+1) + x + 1;
		}
	mesh->UnlockIndexBuffer();

	mesh->LockVertexBuffer( D3DLOCK_NOSYSLOCK, (void **)&mv);
	for( z = 0, i = 0; z <= zgrid; z++)
		for( x = 0; x <= xgrid; x++, i++)
		{
			zeile = ((height-1)*(zgrid-z))/zgrid;
			spalte = ((width-1)*x)/xgrid;
			gw = grauwert( *(((D3DCOLOR *)lr.pBits) + width*zeile + spalte));

			mv[i].pos.x = Step*x-CenterX;
			mv[i].pos.y = 0.1*gw;
			mv[i].pos.z = Step*z-CenterZ;
		}
	mesh->UnlockVertexBuffer();
	
	hm->UnlockRect( 0);
	if( hm)hm->Release();


	LPSPERIGIDBODY	pBody;
	LPSPESHAPE pShape = pWorld->CreateShape();
	InitShape(pShape, mesh);
	pBody = pWorld->AddRigidBody (pShape);
	pBody->SetPosition(SPEVector(0.0f,0.0f,0.0f));
	pBody->SetBeStatic(true);
	pWorld->ReleaseShape(pShape);
	mesh->Release();
}

# endif


