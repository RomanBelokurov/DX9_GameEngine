#pragma once

/* THIS PART OF CODE IS NOT FINISHED YET */

#ifndef __RESOURCE_MANAGER__H__
#define __RESOURCE_MANAGER__H__

#include	<d3dx9.h>
#include	<d3d9.h>
#include	<d3dx9math.h>
#include	<d3dx9mesh.h>
#include 	<vector>

class ResourceManager
{
public:
	std::vector<LPDIRECT3DTEXTURE9>	RM_TEXTURE; // texture
	std::vector<LPD3DXMESH>			RM_MESH; // mesh
	std::vector<DWORD>				MATNUM; // material

	ResourceManager();
	~ResourceManager();


	int LoadTexture(IDirect3DDevice9* m_device, const char* ContainerFilename, const char* Name);
	int LoadMesh(IDirect3DDevice9* m_device, const char* ContainerFilename, const char* Name);
	void ComputeTangent(IDirect3DDevice9* m_device, int ID);


	void DeleteMesh(int ID);
	void DeleteTexture(int ID);
	
	void Clear();

private:
	void ExtractMesh(const char* ContainerFile, const char* MeshNameInContainer, const char* FileToSave);
	void ExtractObject(const char* ContainerFile, const char* TextureName, const char* FileToSave);
};

ResourceManager::ResourceManager()
{
	RM_MESH.clear();
	RM_TEXTURE.clear();
	MATNUM.clear();
}

ResourceManager::~ResourceManager()
{
}


int ResourceManager::LoadTexture(IDirect3DDevice9* m_device, const char* ContainerFilename, const char* Name)
{
	LPDIRECT3DTEXTURE9 temp;
	ExtractObject(ContainerFilename, Name, "TMP.img");
	
	if (FAILED(D3DXCreateTextureFromFileEx(m_device, "TMP.img", 512, 512, 3, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, NULL, NULL, NULL, &temp)))
		exit(0);

	remove("TMP.img");
	RM_TEXTURE.push_back(temp);
	return (int)RM_TEXTURE.size();
}


int ResourceManager::LoadMesh(IDirect3DDevice9* m_device, const char* ContainerFilename, const char* Name)
{
	LPD3DXMESH temp = NULL;
	DWORD tempmatnum = 0;

	LPD3DXBUFFER ppAdjacencyBuffer = NULL;
	ExtractMesh(ContainerFilename, Name, "TMP.x");

	if (FAILED(D3DXLoadMeshFromX("TMP.x", D3DPOOL_DEFAULT, m_device, &ppAdjacencyBuffer, NULL, NULL, &tempmatnum, &temp)))
		exit(0);

	temp->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)ppAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL);
	
	RM_MESH.push_back(temp);
	MATNUM.push_back(tempmatnum);
	remove("TMP.x");
	return 0;
}

void ResourceManager::ComputeTangent(IDirect3DDevice9* m_device, int ID)
{
	D3DVERTEXELEMENT9 Elements[] =
	{
		{ 0, sizeof(float) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, sizeof(float) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, sizeof(float) * 6, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, sizeof(float) * 8, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, sizeof(float) * 11, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};
	//Compute the tangent frame for the mesh. (Needed for normal mapping)
	ID3DXMesh* Temp;
	RM_MESH[ID]->CloneMesh(D3DXMESH_MANAGED, Elements, m_device, &Temp);
	RM_MESH[ID]->Release();
	RM_MESH[ID] = Temp;
	void* Data;
	RM_MESH[ID]->LockVertexBuffer(D3DLOCK_DISCARD, &Data);
	RM_MESH[ID]->UnlockVertexBuffer();
	D3DXComputeTangentFrame(RM_MESH[ID], 0);
}


void ResourceManager::ExtractObject(const char* ContainerFile, const char* TextureName, const char* FileToSave)
{
	FILE* fp, * FRESULT;
	int token, startpos, length, namelen;
	char buf[25] = "";
	char DATA;

	fp = fopen(ContainerFile, "rb");
	fread(&token, sizeof(int), 1, fp);

	for (int i = 0; i < token; i++)
	{
		ZeroMemory(&buf, sizeof(buf));
		fread(&namelen, sizeof(int), 1, fp);
		fread(&buf, sizeof(char), namelen, fp);
		fread(&startpos, sizeof(int), 1, fp);
		fread(&length, sizeof(int), 1, fp);

		if (strcmp(TextureName, buf) == 0)
		{
			FRESULT = fopen(FileToSave, "wb");
			fseek(fp, startpos, 0);

			for (int j = startpos; j < length; j++)
			{
				putc(getc(fp), FRESULT);
			}
			if (FRESULT != NULL)fclose(FRESULT);
			if (fp != NULL)fclose(fp);

			break;//goto END;
		}
	}
	if (fp != NULL)fclose(fp);
}

void ResourceManager::ExtractMesh(const char* ContainerFile, const char* MeshNameInContainer, const char* FileToSave)
{
	int token;
	FILE* FOUT = fopen(FileToSave, "wb");
	FILE* FIN = fopen(ContainerFile, "rb");
	fread(&token, sizeof(int), 1, FIN);

	unsigned int MeshNameLen = 0, MeshMatLen = 0, fsize = 0, startptr = 0;
	char MeshName[50] = "", MaterialString[256] = "";

	for (int i = 0; i < token; i++)
	{
		ZeroMemory(&MeshName, sizeof(MeshName));
		ZeroMemory(&MaterialString, sizeof(MaterialString));

		fread(&MeshNameLen, sizeof(unsigned int), 1, FIN);
		fread(&MeshName, sizeof(char), MeshNameLen, FIN);

		fread(&MeshMatLen, sizeof(unsigned int), 1, FIN);
		fread(&MaterialString, sizeof(char), MeshMatLen, FIN);

		fread(&startptr, sizeof(unsigned int), 1, FIN);
		fread(&fsize, sizeof(unsigned int), 1, FIN);

		if (strcmp(MeshNameInContainer, MeshName) == 0)
		{
			//FOUT=fopen(FileToSave,"wb");
			fseek(FIN, (int)startptr, 0);

			for (int j = 0; j < fsize; j++)
			{
				putc(getc(FIN), FOUT);
			}

			if (FOUT != NULL)fclose(FOUT);
			if (FIN != NULL)fclose(FIN);
			break;
		}
	}
	if (FIN != NULL)fclose(FIN);
	if (FOUT != NULL)fclose(FOUT);
}


void ResourceManager::DeleteMesh(int ID)
{
	RM_MESH[ID]->Release();
	RM_MESH[ID] = 0;
	RM_MESH.erase(RM_MESH.begin() + ID);
}

void ResourceManager::DeleteTexture(int ID)
{
	RM_TEXTURE[ID]->Release();
	RM_TEXTURE[ID] = 0;	
	RM_TEXTURE.erase(RM_TEXTURE.begin() + ID);
}

void ResourceManager::Clear()
{
	for (int i = 0; i < (int)RM_MESH.size(); i++) 
		DeleteMesh(i);
	RM_MESH.clear();

	for (int i = 0; i < (int)RM_TEXTURE.size(); i++)
		DeleteTexture(i);
	RM_TEXTURE.clear();

	MATNUM.clear();
}

#endif