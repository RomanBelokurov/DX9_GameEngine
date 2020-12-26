#ifndef __LGS_SAVELOAD__H__
#define __LGS_SAVELOAD__H__
#include	<math.h>
#include	<windows.h>
#include	<mmsystem.h>
#include	<d3dx9.h>
#include	<d3d9.h>
#include	<windowsx.h>
#include	<d3dx9math.h>
#include	<d3dx9mesh.h>
#include	<stdio.h>
#include	<string>
using namespace std;



ResourceManager LoadScene(ResourceManager RMLoad, const char* Filename, const char* SplashFileName)
{
	DXSprite splash;
	DXText text_data;
	splash.LoadSprite(dx9Driver.VPORT1,SplashFileName, dx9Driver.RConf.screenWidth-150, dx9Driver.RConf.screenHeight-150,D3DCOLOR_XRGB(0,0,0));
	text_data.Setup2DFont(dx9Driver.VPORT1, "Arial",20, FW_NORMAL);

	RMLoad.CLEAN();
	TERRAIN_LOADED_ALLREADY=FALSE;

	OBJECT_VECTOR.clear();
	INDEP_OBJ.clear();
	OBJ_COUNTER=0;
	SELECTED_MESH=0;

	unsigned long fsize1,fsize2,fsize3,fsize4,fsize5;
	float float_data1,float_data2,float_data3;
	int int_data1,int_data2,int_data3;
	int d1,d2;
	byte b;

	dx9Driver.Begin(D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
		splash.DrawSprite(V3(75,75,0),D3DCOLOR_XRGB(255,255,255));
		text_data.Draw2DText("Loading: Sun & Sky parameters",75, dx9Driver.RConf.screenHeight*0.93f,D3DCOLOR_XRGB(0,155,255));
	dx9Driver.End();


	FILE *FIN,*FOUT;
	FIN=fopen(Filename,"rb");

	fread(&int_data1,sizeof(int),1,FIN);
	fread(&int_data2,sizeof(int),1,FIN);

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);

	fread(&int_data3,sizeof(int),1,FIN); SKYDOME.SetSunScale(int_data3);
	fread(&int_data3,sizeof(int),1,FIN); SKYDOME.hour=int_data3;
	fread(&int_data3,sizeof(int),1,FIN); SKYDOME.SetCloudSpeed(int_data3);
	fread(&int_data3,sizeof(int),1,FIN); SKYDOME.SetAlphaClouds(int_data3); 

	SKYDOME.LightColor=V3(float_data1,float_data2,float_data3);
	SKYDOME.LightPosition = SphereTrans(V3(0,0,0),V3(0.0f,0.0f,-700.0f),V3(int_data1,int_data2,int_data1));
	SKYDOME.rotation=int_data2;
	SKYDOME.SetTime(int_data1);
	SKYDOME.SetLight();
	Terr.Update();

	fread(&d1,sizeof(int),1,FIN);
	if(d1>0)
	{
		FOUT=fopen("Cloud","wb");
		for(int i=0; i<d1; i++) putc(getc(FIN),FOUT);
		if(FOUT!=NULL)fclose(FOUT);
		SKYDOME.LoadCloudTexture("Cloud");
		remove("Cloud");
	}

//******************************************************************************
	dx9Driver.Begin(D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
	splash.DrawSprite(V3(75,75,0),D3DCOLOR_XRGB(255,255,255));
	text_data.Draw2DText("Loading: Fog parameters",75, dx9Driver.RConf.screenHeight*0.93f,D3DCOLOR_XRGB(0,155,255));
	dx9Driver.End();

	fread(&float_data1,sizeof(float),1,FIN);
	fread(&float_data2,sizeof(float),1,FIN);
	fread(&float_data3,sizeof(float),1,FIN);
	
	D3DXVECTOR3 GLOBAL_FOG_COLOR=V3(float_data1,float_data2,float_data3);

	fread(&int_data1,sizeof(int),1,FIN);
	fread(&int_data2,sizeof(int),1,FIN);
	int GLOBAL_FOG_MIN=int_data1;
	int GLOBAL_FOG_MAX=int_data2;
	
	dx9Driver.SetFog(D3DCOLOR_XRGB((int)GLOBAL_FOG_COLOR.x,(int)GLOBAL_FOG_COLOR.y,(int)GLOBAL_FOG_COLOR.z),GLOBAL_FOG_MIN,GLOBAL_FOG_MAX);


//************************ TERRAIN_SYSTEM **************************************
	dx9Driver.Begin(D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
	splash.DrawSprite(V3(75,75,0),D3DCOLOR_XRGB(255,255,255));
	text_data.Draw2DText("Loading: Terrain generation",75, dx9Driver.RConf.screenHeight*0.93f,D3DCOLOR_XRGB(0,155,255));
	dx9Driver.End();

	fread(&TERRAIN_FOV,sizeof(int),1,FIN);
	fread(&fsize1,sizeof(int),1,FIN);
	fread(&fsize2,sizeof(int),1,FIN);
	fread(&fsize3,sizeof(int),1,FIN);
	fread(&fsize4,sizeof(int),1,FIN);
	fread(&fsize5,sizeof(int),1,FIN);

	FOUT=fopen("~temp_hm","wb");
	for(int i=0;i<fsize1;i++)
	{
		putc(getc(FIN),FOUT);
	}
	if(FOUT!=NULL)fclose(FOUT);

	FOUT=fopen("~temp_blend","wb");
	for(int i=0;i<fsize2;i++)
	{
		putc(getc(FIN),FOUT);
	}
	if(FOUT!=NULL)fclose(FOUT);

	FOUT=fopen("~temp_layer1","wb");
	for(int i=0;i<fsize3;i++)
	{
		putc(getc(FIN),FOUT);
	}
	if(FOUT!=NULL)fclose(FOUT);
	FOUT=fopen("~temp_layer2","wb");
	for(int i=0;i<fsize4;i++)
	{
		putc(getc(FIN),FOUT);
	}
	if(FOUT!=NULL)fclose(FOUT);
	FOUT=fopen("~temp_layer3","wb");
	for(int i=0;i<fsize5;i++)
	{
		putc(getc(FIN),FOUT);
	}
	if(FOUT!=NULL)fclose(FOUT);

	fread(&d1,sizeof(int),1,FIN);fread(&d2,sizeof(int),1,FIN);		//L,W
	fread(&int_data1,sizeof(int),1,FIN);fread(&int_data2,sizeof(int),1,FIN);//Meshstep , division
	
	if(d1+d2>0)
	{
		CreateTerrainPhysics(dx9Driver.VPORT1,"~temp_hm", d1,d2, int_data1);
		Terr.BildTerrain(dx9Driver.VPORT1,"~temp_hm",d1,d2,int_data2,int_data1);
		Terr.LoadTextures("~temp_blend","~temp_layer1","~temp_layer2","~temp_layer3");
		FIND_SELF_IN_TERRAIN(VP1Camera,TERRAIN_FOV);
	}
	remove("~temp_hm");
	remove("~temp_blend");
	remove("~temp_layer1");remove("~temp_layer2");remove("~temp_layer3");
//******************************************************************************


//**************************** RESOURCEMANAGER_SYSTEM **************************
//MESHES
	dx9Driver.Begin(D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
	splash.DrawSprite(V3(75,75,0),D3DCOLOR_XRGB(255,255,255));
	text_data.Draw2DText("Loading: Resource manager(meshes)",75, dx9Driver.RConf.screenHeight*0.93f,D3DCOLOR_XRGB(0,155,255));
	dx9Driver.End();

	int RecordLength=0,p;
	string RecordString="",FirstPart="";
	char ch;

	fread(&d1,sizeof(int),1,FIN);
	for(int i=0;i<d1;i++)
	{
		fread(&RecordLength,sizeof(int),1,FIN);
		RecordString="";FirstPart="";

		for(int j=0;j<RecordLength;j++)
		{
			fread(&ch,sizeof(char),1,FIN);
			RecordString+=ch;
		}

		FirstPart=RecordString;
		FirstPart.erase(RecordString.find("->",0),RecordString.length());
		
		RecordString.erase(0,RecordString.find("->",0)+2);
		FirstPart="System\\StaticMesh\\"+FirstPart;
		RMLoad.LoadMesh(FirstPart.c_str(),RecordString.c_str());
	}

//TEXTURES
	dx9Driver.Begin(D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
	splash.DrawSprite(V3(75,75,0),D3DCOLOR_XRGB(255,255,255));
	text_data.Draw2DText("Loading: Resource manager(textures)",75, dx9Driver.RConf.screenHeight*0.93f,D3DCOLOR_XRGB(0,155,255));
	dx9Driver.End();

	fread(&d1,sizeof(int),1,FIN);
	for(int i=0;i<d1;i++)
	{
		fread(&RecordLength,sizeof(int),1,FIN);
		RecordString="";FirstPart="";

		for(int j=0;j<RecordLength;j++)
		{
			fread(&ch,sizeof(char),1,FIN);
			RecordString+=ch;
		}

		FirstPart=RecordString;
		FirstPart.erase(RecordString.find("->",0),RecordString.length());
		
		RecordString.erase(0,RecordString.find("->",0)+2);
		FirstPart="System\\Textures\\"+FirstPart;
		
		RMLoad.LoadTexture(FirstPart.c_str(),RecordString.c_str());		
	}
//******************************************************************************

//************************** OBJECT VECTOR LOAD ********************************
	dx9Driver.Begin(D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
	splash.DrawSprite(V3(75,75,0),D3DCOLOR_XRGB(255,255,255));
	text_data.Draw2DText("Loading: Objects",75, dx9Driver.RConf.screenHeight*0.93f,D3DCOLOR_XRGB(0,155,255));
	dx9Driver.End();

	Object OBJECT;
	fread(&d1,sizeof(int),1,FIN);

	for(int i=0;i<d1;i++)
	{
		fread(&int_data1,sizeof(int),1,FIN);
		OBJECT.MeshID=int_data1;		
		OBJECT.ComputeBoundingBox(dx9Driver.VPORT1,RMLoad);
		OBJECT.Colide=TRUE;
		OBJECT.DropShadow=FALSE;

		fread(&int_data2,sizeof(int),1,FIN);
		OBJECT.TextureLayer_0.clear();
		for(int j=0;j<int_data2;j++)
		{
			fread(&int_data3,sizeof(int),1,FIN);
			OBJECT.TextureLayer_0.push_back(int_data3);
		}

		fread(&float_data1,sizeof(float),1,FIN);
		fread(&float_data2,sizeof(float),1,FIN);
		fread(&float_data3,sizeof(float),1,FIN);
		OBJECT.Position=V3(float_data1,float_data2,float_data3);

		fread(&float_data1,sizeof(float),1,FIN);
		fread(&float_data2,sizeof(float),1,FIN);
		fread(&float_data3,sizeof(float),1,FIN);
		OBJECT.Scalling=V3(float_data1,float_data2,float_data3);

		fread(&float_data1,sizeof(float),1,FIN);
		fread(&float_data2,sizeof(float),1,FIN);
		fread(&float_data3,sizeof(float),1,FIN);
		OBJECT.Rotation=V3(float_data1,float_data2,float_data3);

		fread(&int_data1,sizeof(int),1,FIN);	OBJECT.FULL_CULLING=int_data1;
		fread(&int_data1,sizeof(int),1,FIN);	if(int_data1==0)OBJECT.Colide=FALSE;
		fread(&int_data1,sizeof(int),1,FIN);	if(int_data1==1)OBJECT.DropShadow=TRUE;

		OBJECT.UpdatePosAABB();
		OBJECT_VECTOR.push_back(OBJECT);
		OBJ_COUNTER++;		
	}	
//******************************************************************************
// INDEPENDENT ROAD LOADING
	dx9Driver.Begin(D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
	splash.DrawSprite(V3(75,75,0),D3DCOLOR_XRGB(255,255,255));
	text_data.Draw2DText("Loading: Additional parameters",75, dx9Driver.RConf.screenHeight*0.93f,D3DCOLOR_XRGB(0,155,255));
	dx9Driver.End();

	fread(&d1,sizeof(int),1,FIN);
	if(d1>0)
	{
		fread(&int_data1,sizeof(int),1,FIN);
		OBJECT.MeshID=int_data1;
		OBJECT.ComputeBoundingBox(dx9Driver.VPORT1,RMLoad);

		fread(&int_data2,sizeof(int),1,FIN);
		OBJECT.TextureLayer_0.clear();
		for(int j=0;j<int_data2;j++)
		{
			fread(&int_data3,sizeof(int),1,FIN);
			OBJECT.TextureLayer_0.push_back(int_data3);
		}

		fread(&float_data1,sizeof(float),1,FIN);
		fread(&float_data2,sizeof(float),1,FIN);
		fread(&float_data3,sizeof(float),1,FIN);
		OBJECT.Position=V3(float_data1,float_data2,float_data3);

		fread(&float_data1,sizeof(float),1,FIN);
		fread(&float_data2,sizeof(float),1,FIN);
		fread(&float_data3,sizeof(float),1,FIN);
		OBJECT.Scalling=V3(float_data1,float_data2,float_data3);

		fread(&float_data1,sizeof(float),1,FIN);
		fread(&float_data2,sizeof(float),1,FIN);
		fread(&float_data3,sizeof(float),1,FIN);
		OBJECT.Rotation=V3(float_data1,float_data2,float_data3);

		fread(&int_data1,sizeof(int),1,FIN);	OBJECT.FULL_CULLING=int_data1;
		OBJECT.UpdatePosAABB();
		INDEP_OBJ.push_back(OBJECT);
	}
//*************************
fread(&d1,sizeof(int),1,FIN);
//*************************
	if(FIN!=NULL)fclose(FIN);
	SortObjectsInQuads();
	
	dx9Driver.Begin(D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0));
	splash.DrawSprite(V3(75,75,0),D3DCOLOR_XRGB(255,255,255));
	text_data.Draw2DText("Physics initialising... Please wait",75, dx9Driver.RConf.screenHeight*0.93f,D3DCOLOR_XRGB(0,155,255));
	dx9Driver.End();
	return RMLoad;
}

# endif __LGS_SAVELOAD__H__
