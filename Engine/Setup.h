#ifndef _SECURE_SCL
#define _SECURE_SCL 0
#endif

#undef max
#undef min

#define STRICT
#define WIN32_LEAN_AND_MEAN		
#define _CRT_SECURE_NO_DEPRECATE
 
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"Vfw32.lib")

#include	<fstream>
#include	<vector>

using namespace std;
#include	<tchar.h>
#include	<windows.h>
#include	<mmsystem.h>
#include	<d3dx9.h>
#include	<d3d9.h>
#include	<windowsx.h>
#include	<d3dx9math.h>
#include	<d3dx9mesh.h>
#include	<cassert>

//Main objects CORE
#include	"EngineCore/LIGA_Def.h"					//Engine common definition
#include	"EngineCore/Driver_DX9.h"

Driver_DX9 dx9Driver;

#include	"EngineCore/LIGA_Camera.h"					//Engine Camera!!!
#include	"EngineCore/LIGA_Input.h"				
#include	"EngineCore/LIGA_Physics.h"	

//Objects of static items
#include	"EngineCore/LIGA_ResMan.h"					// Resource manager
#include	"EngineCore/LIGA_Sky.h"					// Resource manager
#include	"EngineCore/LIGA_Object.h"					// Object manager
#include	"EngineCore/LIGA_QuadTree.h"				// QuadTree system
#include	"EngineCore/LIGA_Terrain.h"				// Terrain engine
#include	"EngineCore/LIGA_PP_Bloom.h"				// Post-Processor
#include	"EngineCore/LIGA_HUD.h"					// HUD System
#include	"EngineCore/LIGA_Partsys.h"					// Particleclass
#include	"EngineCore/LIGA_SaveLoad.h"				// Extractor


#include	"Player/Player.h"					// PlayerClass
#include	"Player/PlayerShoot.h"
#include	"Player/PlayerCamera.h"




