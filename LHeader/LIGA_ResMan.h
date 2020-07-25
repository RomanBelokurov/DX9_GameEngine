#ifndef __LGS_RM__H__
#define __LGS_RM__H__

#include	<math.h>
#include	<windows.h>
#include	<mmsystem.h>
#include	<d3dx9.h>
#include	<d3d9.h>
#include	<windowsx.h>
#include	<d3dx9math.h>
#include	<d3dx9mesh.h>
#include	<stdio.h>

#include 	<vector>
using namespace std;

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }
#define V3 D3DXVECTOR3
#define DEG_TO_RAD(ang) ((ang)*D3DX_PI/180)

void ExtractObject(const char* ContainerFile,const char* TextureName,const char* FileToSave)
{
	FILE *fp,*FRESULT;
	int token,startpos,length,namelen;
	char buf[25]="";
	char DATA;

	fp=fopen(ContainerFile,"rb");
	fread(&token,sizeof(int),1,fp);

	for(int i=0;i<token;i++)
	{
		ZeroMemory(&buf,sizeof(buf));
		fread(&namelen,sizeof(int),1,fp);
		fread(&buf,sizeof(char),namelen,fp);
		fread(&startpos,sizeof(int),1,fp);
		fread(&length,sizeof(int),1,fp);

		if(strcmp(TextureName,buf)==0)
		{
			FRESULT=fopen(FileToSave,"wb");
			fseek(fp,startpos,0);

			for(int j=startpos;j<length;j++)
			{
				putc(getc(fp),FRESULT);
			}
			if(FRESULT!=NULL)fclose(FRESULT);
			if(fp!=NULL)fclose(fp);

			break;//goto END;
		}
	}
	if(fp!=NULL)fclose(fp);
}

void ExtractMesh(const char* ContainerFile,const char* MeshNameInContainer,const char* FileToSave)
{
	int token;
	FILE *FOUT = fopen(FileToSave, "wb");
	FILE* FIN = fopen(ContainerFile, "rb");
	fread(&token,sizeof(int),1,FIN);

	unsigned int MeshNameLen=0,MeshMatLen=0,fsize=0,startptr=0;
	char MeshName[50]="",MaterialString[256]="";

	for(int i=0;i<token;i++)
	{
		ZeroMemory(&MeshName,sizeof(MeshName));
		ZeroMemory(&MaterialString,sizeof(MaterialString));

		fread(&MeshNameLen,sizeof(unsigned int),1,FIN);
		fread(&MeshName,sizeof(char),MeshNameLen,FIN);

		fread(&MeshMatLen,sizeof(unsigned int),1,FIN);
		fread(&MaterialString,sizeof(char),MeshMatLen,FIN);

		fread(&startptr,sizeof(unsigned int),1,FIN);
		fread(&fsize,sizeof(unsigned int),1,FIN);

		if(strcmp(MeshNameInContainer,MeshName)==0)
		{
			//FOUT=fopen(FileToSave,"wb");
			fseek(FIN,(int)startptr,0);

			for(int j=0;j<fsize;j++)
			{
				putc(getc(FIN),FOUT);
			}

			if(FOUT!=NULL)fclose(FOUT);
			if(FIN!=NULL)fclose(FIN);
			break;
		}
	}
	if(FIN!=NULL)fclose(FIN);
	if(FOUT!=NULL)fclose(FOUT);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ResourceManager
{
   LPDIRECT3DDEVICE9 		g_pd3dDevice;
public:
   vector<LPDIRECT3DTEXTURE9>	RM_TEXTURE;
   vector<LPD3DXMESH>			RM_MESH;
   vector<DWORD>				MATNUM;

   ResourceManager();
   void CLEAN();
   void SetDevice(LPDIRECT3DDEVICE9 Device){g_pd3dDevice=Device;}
   int LoadTexture(const char* ContainerFilename,const char* Name);
   int LoadMesh(const char* ContainerFilename,const char* Name);

   void DeleteMesh(int ID);
   void DeleteTexture(int ID);

   void ComputeTangent(int ID);
};

ResourceManager RM;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void ResourceManager::DeleteMesh(int ID)
{
	ReleaseCOM(RM_MESH[ID])
	RM_MESH.erase(RM_MESH.begin()+ID);
}

void ResourceManager::DeleteTexture(int ID)
{
	ReleaseCOM(RM_TEXTURE[ID])
	RM_TEXTURE.erase(RM_TEXTURE.begin()+ID);
}

ResourceManager::ResourceManager()
{
	RM_MESH.clear();
	RM_TEXTURE.clear();
	MATNUM.clear();
}

void ResourceManager::CLEAN()
{
	for(int i=0;i<(int)RM_MESH.size();i++)ReleaseCOM(RM_MESH[i]);		RM_MESH.clear();
	for(int i=0;i<(int)RM_TEXTURE.size();i++)ReleaseCOM(RM_TEXTURE[i]);	RM_TEXTURE.clear();
	MATNUM.clear();
}

int ResourceManager::LoadTexture(const char* ContainerFilename,const char* Name)
{
	LPDIRECT3DTEXTURE9 temp;
	ExtractObject(ContainerFilename,Name,"TMP.img");
	if(g_pd3dDevice==NULL)MessageBox(0, _T("Error device not created"), _T("Error"),NULL);
	if(FAILED(D3DXCreateTextureFromFileEx(g_pd3dDevice,"TMP.img",512,512,3,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,NULL,NULL,NULL,&temp)))
		exit(0);

	remove("TMP.img");
	RM_TEXTURE.push_back(temp);
	return (int)RM_TEXTURE.size();
}


int ResourceManager::LoadMesh(const char* ContainerFilename,const char* Name)
{
	LPD3DXMESH temp=NULL;
	DWORD tempmatnum=0;

	LPD3DXBUFFER ppAdjacencyBuffer=NULL;
	ExtractMesh(ContainerFilename,Name,"TMP.x");
	if(g_pd3dDevice==NULL)MessageBox(0, _T("Error device not created"), _T("Error"),NULL);

	if(FAILED(D3DXLoadMeshFromX("TMP.x", D3DPOOL_DEFAULT,g_pd3dDevice, &ppAdjacencyBuffer, NULL,NULL, &tempmatnum, &temp)))exit(0);
	temp->OptimizeInplace(D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_COMPACT|D3DXMESHOPT_VERTEXCACHE,(DWORD*)ppAdjacencyBuffer->GetBufferPointer(),NULL,NULL,NULL);
	RM_MESH.push_back(temp);
	MATNUM.push_back(tempmatnum);
	remove("TMP.x");
	return 0;
}

void ResourceManager::ComputeTangent(int ID)
{
	D3DVERTEXELEMENT9 Elements[] =
	{
		{ 0, sizeof( float ) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, sizeof( float ) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, sizeof( float ) * 6, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, sizeof( float ) * 8, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, sizeof( float ) * 11, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};
				//Compute the tangent frame for the mesh. (Needed for normal mapping)
	ID3DXMesh* Temp;
	RM_MESH[ID]->CloneMesh( D3DXMESH_MANAGED, Elements, g_pd3dDevice, &Temp );
	RM_MESH[ID]->Release();
	RM_MESH[ID] = Temp;
	void* Data;
	RM_MESH[ID]->LockVertexBuffer(D3DLOCK_DISCARD, &Data);
	RM_MESH[ID]->UnlockVertexBuffer();
	D3DXComputeTangentFrame(RM_MESH[ID], 0);
}



#endif __LGS_RM__H__

