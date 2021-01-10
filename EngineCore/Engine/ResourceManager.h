
/* THIS PART OF CODE IS from old implementation and bad to use */

#ifndef __RESOURCE_MANAGER__H__
#define __RESOURCE_MANAGER__H__
#pragma once



#include	<d3dx9.h>
#include	<d3d9.h>
#include	<d3dx9math.h>
#include	<d3dx9mesh.h>

#include 	<vector>
#include	<fstream>

#include "StaticObject.h"

class ResourceManager
{
public:
	std::vector<LPDIRECT3DTEXTURE9>	RM_TEXTURE; // texture
	std::vector<LPD3DXMESH>			RM_MESH; // mesh
	std::vector<DWORD>				MATNUM; // material

	std::vector<StaticObject>		OBJECT_VECTOR;

	ResourceManager();
	~ResourceManager();

	void LoadSceneFromFile(IDirect3DDevice9* m_device, std::string filename);


	int LoadTexture(IDirect3DDevice9* m_device, std::string ContainerFilename, std::string Name);
	int LoadMesh(IDirect3DDevice9* m_device, std::string ContainerFilename, std::string Name);
	void ComputeTangent(IDirect3DDevice9* m_device, int ID);


	void DeleteMesh(int ID);
	void DeleteTexture(int ID);
	
	void Clear();

private:
	// this extracts datd from container files
	bool ExtractMesh(std::string, std::string, std::string);
	bool ExtractTexture(std::string, std::string, std::string);
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


// Used old implementation, because of old file format
// Todo: Remove it later
void ResourceManager::LoadSceneFromFile(IDirect3DDevice9* m_device, std::string filename)
{
	unsigned long fsize1, fsize2, fsize3, fsize4, fsize5;
	float float_data1, float_data2, float_data3;
	int int_data1, int_data2, int_data3;
	int d1, d2;
	byte b;

	std::ifstream input(filename, std::ios::binary);

	input.read(reinterpret_cast<char*>(&int_data1), sizeof(int_data1));
	input.read(reinterpret_cast<char*>(&int_data2), sizeof(int_data2));

	input.read(reinterpret_cast<char*>(&float_data1), sizeof(float_data1));
	input.read(reinterpret_cast<char*>(&float_data2), sizeof(float_data2));
	input.read(reinterpret_cast<char*>(&float_data3), sizeof(float_data3));

	// SKYDOME stuff
	input.read(reinterpret_cast<char*>(&int_data3), sizeof(int_data3)); //SKYDOME.SetSunScale(int_data3);
	input.read(reinterpret_cast<char*>(&int_data3), sizeof(int_data3)); //SKYDOME.hour = int_data3;
	input.read(reinterpret_cast<char*>(&int_data3), sizeof(int_data3)); //SKYDOME.SetCloudSpeed(int_data3);
	input.read(reinterpret_cast<char*>(&int_data3), sizeof(int_data3)); //SKYDOME.SetAlphaClouds(int_data3);
	
	/*SKYDOME.LightColor = V3(float_data1, float_data2, float_data3);
	SKYDOME.LightPosition = SphereTrans(V3(0, 0, 0), V3(0.0f, 100.0f, -700.0f), V3(int_data1, int_data2, int_data1));
	SKYDOME.rotation = int_data2;
	SKYDOME.SetTime(int_data1);
	SKYDOME.SetLight();
	Terr.Update();*/

	input.read(reinterpret_cast<char*>(&d1), sizeof(int));
	if (d1 > 0)
	{
		// cloud texture
		std::ofstream output("cloud.png", std::fstream::trunc | std::fstream::binary);
		
		char* pData = (char*)std::calloc(d1, sizeof(char));
		input.read(reinterpret_cast<char*>(pData), d1);
		output.write(reinterpret_cast<char*>(pData), d1);
		std::free(pData);
		output.close();
		//SKYDOME.LoadCloudTexture("cloud.png");
		remove("cloud.png");
	}
	
	//*************************** FOG SETTING *************************************
	input.read(reinterpret_cast<char*>(&float_data1), sizeof(float_data1));
	input.read(reinterpret_cast<char*>(&float_data2), sizeof(float_data2));
	input.read(reinterpret_cast<char*>(&float_data3), sizeof(float_data3));
	//GLOBAL_FOG_COLOR = V3(float_data1, float_data2, float_data3);

	input.read(reinterpret_cast<char*>(&int_data1), sizeof(int_data1));
	input.read(reinterpret_cast<char*>(&int_data2), sizeof(int_data2));
	//GLOBAL_FOG_MIN = int_data1; 
	//GLOBAL_FOG_MAX = int_data2;
	//SetFog(VPORT1, D3DCOLOR_XRGB((int)GLOBAL_FOG_COLOR.x, (int)GLOBAL_FOG_COLOR.y, (int)GLOBAL_FOG_COLOR.z), GLOBAL_FOG_MIN, GLOBAL_FOG_MAX);

	//************************ TERRAIN_SYSTEM **************************************	
	unsigned long TERRAIN_FOV;
	input.read(reinterpret_cast<char*>(&TERRAIN_FOV), sizeof(TERRAIN_FOV));
	input.read(reinterpret_cast<char*>(&fsize1), sizeof(fsize1));
	input.read(reinterpret_cast<char*>(&fsize2), sizeof(fsize2));
	input.read(reinterpret_cast<char*>(&fsize3), sizeof(fsize3));
	input.read(reinterpret_cast<char*>(&fsize4), sizeof(fsize4));
	input.read(reinterpret_cast<char*>(&fsize5), sizeof(fsize5));

	if (fsize1 > 0) 
	{
		std::ofstream output("~temp_hm.png", std::fstream::trunc | std::fstream::binary);
		char* pData = (char*)std::calloc(fsize1, sizeof(char));
		input.read(reinterpret_cast<char*>(pData), fsize1);
		output.write(reinterpret_cast<char*>(pData), fsize1);
		std::free(pData);
		output.close();
	}

	if (fsize2 > 0)
	{
		std::ofstream output("~temp_blend.png", std::fstream::trunc | std::fstream::binary);
		char* pData = (char*)std::calloc(fsize2, sizeof(char));
		input.read(reinterpret_cast<char*>(pData), fsize2);
		output.write(reinterpret_cast<char*>(pData), fsize2);
		std::free(pData);
		output.close();
	}

	if (fsize3 > 0)
	{
		std::ofstream output("~temp_layer1.png", std::fstream::trunc | std::fstream::binary);
		char* pData = (char*)std::calloc(fsize3, sizeof(char));
		input.read(reinterpret_cast<char*>(pData), fsize3);
		output.write(reinterpret_cast<char*>(pData), fsize3);
		std::free(pData);
		output.close();
	}

	if (fsize4 > 0)
	{
		std::ofstream output("~temp_layer2.png", std::fstream::trunc | std::fstream::binary);
		char* pData = (char*)std::calloc(fsize4, sizeof(char));
		input.read(reinterpret_cast<char*>(pData), fsize4);
		output.write(reinterpret_cast<char*>(pData), fsize4);
		std::free(pData);
		output.close();
	}

	if (fsize5 > 0)
	{
		std::ofstream output("~temp_layer3.png", std::fstream::trunc | std::fstream::binary);
		char* pData = (char*)std::calloc(fsize5, sizeof(char));
		input.read(reinterpret_cast<char*>(pData), fsize5);
		output.write(reinterpret_cast<char*>(pData), fsize5);
		std::free(pData);
		output.close();
	}
	
	input.read(reinterpret_cast<char*>(&d1), sizeof(d1)); // terrain size length
	input.read(reinterpret_cast<char*>(&d2), sizeof(d2)); // terrain size width
	input.read(reinterpret_cast<char*>(&int_data1), sizeof(int_data1)); // mesh step
	input.read(reinterpret_cast<char*>(&int_data2), sizeof(int_data2)); // mesh division

	if (d1 + d2 > 0)
	{
		//CreateTerrainPhysics(VPORT1, "~temp_hm", d1, d2, int_data1);
		//Terr.BildTerrain(VPORT1, "~temp_hm", d1, d2, int_data2, int_data1);
		//Terr.LoadTextures("~temp_blend", "~temp_layer1", "~temp_layer2", "~temp_layer3");
		//FIND_SELF_IN_TERRAIN(VP1Camera, TERRAIN_FOV);
	}
	remove("~temp_hm.png");
	remove("~temp_blend.png");
	remove("~temp_layer1.png"); 
	remove("~temp_layer2.png"); 
	remove("~temp_layer3.png");
	//******************************************************************************

	//**************************** RESOURCEMANAGER_SYSTEM **************************
	//MESHES
	int RecordLength = 0;
	char buffer[128] = "";

	input.read(reinterpret_cast<char*>(&d1), sizeof(d1));
	for (int i = 0; i < d1; i++)
	{
		input.read(reinterpret_cast<char*>(&RecordLength), sizeof(RecordLength));
		
		ZeroMemory(&buffer, sizeof(buffer));
		input.read(reinterpret_cast<char*>(&buffer), RecordLength);

		std::string RecordString = buffer;
		std::string FirstPart = RecordString;

		FirstPart.erase(RecordString.find("->", 0), RecordString.length());

		RecordString.erase(0, RecordString.find("->", 0) + 2);
		FirstPart = "../System/StaticMesh/" + FirstPart;
		LoadMesh(m_device, FirstPart.c_str(), RecordString.c_str());
	}

	//TEXTURES
	input.read(reinterpret_cast<char*>(&d1), sizeof(d1));
	for (int i = 0; i < d1; i++)
	{
		input.read(reinterpret_cast<char*>(&RecordLength), sizeof(RecordLength));

		ZeroMemory(&buffer, sizeof(buffer));
		input.read(reinterpret_cast<char*>(&buffer), RecordLength);

		std::string RecordString = buffer;
		std::string FirstPart = RecordString;

		FirstPart.erase(RecordString.find("->", 0), RecordString.length());

		RecordString.erase(0, RecordString.find("->", 0) + 2);
		FirstPart = "../System/Textures/" + FirstPart;

		LoadTexture(m_device, FirstPart.c_str(), RecordString.c_str());
	}
	//******************************************************************************

	//************************** OBJECT VECTOR LOAD ********************************
	StaticObject OBJECT;
	input.read(reinterpret_cast<char*>(&d1), sizeof(d1));

	for (int i = 0; i < d1; i++)
	{
		input.read(reinterpret_cast<char*>(&int_data1), sizeof(int_data1));
		OBJECT.MeshID = int_data1;
		//OBJECT.ComputeBoundingBox(VPORT1, RMLoad);
		//OBJECT.Colide = TRUE;
		//OBJECT.DropShadow = FALSE;

		input.read(reinterpret_cast<char*>(&int_data2), sizeof(int_data2));
		OBJECT.TextureLayer_0.clear();
		for (int j = 0; j < int_data2; j++)
		{			
			input.read(reinterpret_cast<char*>(&int_data3), sizeof(int_data3));
			OBJECT.TextureLayer_0.push_back(int_data3);
		}

		input.read(reinterpret_cast<char*>(&float_data1), sizeof(float_data1));
		input.read(reinterpret_cast<char*>(&float_data2), sizeof(float_data2));
		input.read(reinterpret_cast<char*>(&float_data3), sizeof(float_data3));
		OBJECT.Position = D3DXVECTOR3(float_data1, float_data2, float_data3);

		input.read(reinterpret_cast<char*>(&float_data1), sizeof(float_data1));
		input.read(reinterpret_cast<char*>(&float_data2), sizeof(float_data2));
		input.read(reinterpret_cast<char*>(&float_data3), sizeof(float_data3));
		OBJECT.Scalling = D3DXVECTOR3(float_data1, float_data2, float_data3);

		input.read(reinterpret_cast<char*>(&float_data1), sizeof(float_data1));
		input.read(reinterpret_cast<char*>(&float_data2), sizeof(float_data2));
		input.read(reinterpret_cast<char*>(&float_data3), sizeof(float_data3));
		OBJECT.Rotation = D3DXVECTOR3(float_data1, float_data2, float_data3);

		input.read(reinterpret_cast<char*>(&int_data1), sizeof(int_data1)); //OBJECT.FULL_CULLING = int_data1;
		input.read(reinterpret_cast<char*>(&int_data1), sizeof(int_data1)); //if (int_data1 == 0)OBJECT.Colide = FALSE;
		input.read(reinterpret_cast<char*>(&int_data1), sizeof(int_data1)); //if (int_data1 == 1)OBJECT.DropShadow = TRUE;

		//OBJECT.UpdatePosAABB();
		OBJECT_VECTOR.push_back(OBJECT);
		//OBJ_COUNTER++;
	}
	//******************************************************************************

	input.close();
}


int ResourceManager::LoadTexture(IDirect3DDevice9* m_device, std::string ContainerFilename, std::string Name)
{
	LPDIRECT3DTEXTURE9 temp;
	if (ExtractTexture(ContainerFilename, Name, "TMP.img"))
	{
		if (FAILED(D3DXCreateTextureFromFileEx(m_device, "TMP.img", 512, 512, 3, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, NULL, NULL, NULL, &temp)))
			exit(0);

		remove("TMP.img");
		RM_TEXTURE.push_back(temp);
	}
	return (int)RM_TEXTURE.size();
}


int ResourceManager::LoadMesh(IDirect3DDevice9* m_device, std::string ContainerFilename, std::string Name)
{
	LPD3DXMESH temp = NULL;
	DWORD tempmatnum = 0;

	LPD3DXBUFFER ppAdjacencyBuffer = NULL;
	if (ExtractMesh(ContainerFilename, Name, "TMP.x"))
	{
		if (FAILED(D3DXLoadMeshFromX("TMP.x", D3DPOOL_DEFAULT, m_device, &ppAdjacencyBuffer, NULL, NULL, &tempmatnum, &temp)))
			exit(0);

		temp->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)ppAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL);

		RM_MESH.push_back(temp);
		MATNUM.push_back(tempmatnum);
		remove("TMP.x");
	}
	return (int)RM_MESH.size();
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



bool ResourceManager::ExtractTexture(std::string ContainerFile, std::string TextureName, std::string FileToSave)
{
	std::ifstream input(ContainerFile, std::ios::binary);
	
	int token, startpos, length, namelen;
	char buf[25] = "";

	input.read(reinterpret_cast<char*>(&token), sizeof(token));
	
	for (int i = 0; i < token; i++)
	{
		ZeroMemory(&buf, sizeof(buf));
		input.read(reinterpret_cast<char*>(&namelen), sizeof(namelen)); // read filename length
		input.read(reinterpret_cast<char*>(&buf), sizeof(char) * namelen); // filename
		input.read(reinterpret_cast<char*>(&startpos), sizeof(startpos)); // index of start position in file
		input.read(reinterpret_cast<char*>(&length), sizeof(length)); // file length in bytes
		
		if (TextureName.compare(buf) == 0) 
		{
			std::ofstream output(FileToSave, std::fstream::trunc | std::fstream::binary);
			input.seekg(startpos);

			char* pData = (char*)std::calloc(length, sizeof(char));
			
			input.read(reinterpret_cast<char*>(pData), length);
			output.write(reinterpret_cast<char*>(pData), length);
			
			std::free(pData);
			output.close();
			input.close();
			return true;			
		}
	}
	input.close();
	return false;
}

bool ResourceManager::ExtractMesh(std::string ContainerFile, std::string MeshNameInContainer, std::string FileToSave)
{
	std::ifstream input(ContainerFile, std::ios::binary);

	int token;
	input.read(reinterpret_cast<char*>(&token), sizeof(token));

	unsigned int MeshNameLen = 0, MeshMatLen = 0, fsize = 0, startptr = 0;
	char MeshName[50] = "", MaterialString[256] = "";

	for (int i = 0; i < token; i++)
	{
		ZeroMemory(&MeshName, sizeof(MeshName));
		ZeroMemory(&MaterialString, sizeof(MaterialString));
		
		input.read(reinterpret_cast<char*>(&MeshNameLen), sizeof(MeshNameLen));
		input.read(reinterpret_cast<char*>(&MeshName), sizeof(char) * MeshNameLen);

		input.read(reinterpret_cast<char*>(&MeshMatLen), sizeof(MeshMatLen));
		input.read(reinterpret_cast<char*>(&MaterialString), sizeof(char) * MeshMatLen);

		input.read(reinterpret_cast<char*>(&startptr), sizeof(startptr));
		input.read(reinterpret_cast<char*>(&fsize), sizeof(fsize));

		if (MeshNameInContainer.compare(MeshName) == 0)
		{
			std::ofstream output(FileToSave, std::fstream::trunc | std::fstream::binary);
			input.seekg(startptr);

			char* pData = (char*)std::calloc(fsize, sizeof(char));
			
			input.read(reinterpret_cast<char*>(pData), fsize);
			output.write(reinterpret_cast<char*>(pData), fsize);
			
			std::free(pData);
			output.close();
			input.close();
			return true;			
		}
	}
	input.close();
	return false;
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