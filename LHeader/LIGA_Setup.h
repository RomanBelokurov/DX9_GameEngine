#ifndef _HAS_ITERATOR_DEBUGGING
#define _HAS_ITERATOR_DEBUGGING 0
#endif

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
#include	"LHeader/LIGA_Def.h"					//Engine common definition
#include	"LHeader/LIGA_Core.h"					//Engine CORE!!!
#include	"LHeader/LIGA_Camera.h"					//Engine Camera!!!
#include	"LHeader/LIGA_Timer.h"					//Engine Timer!!!
#include	"LHeader/LIGA_Input.h"				
#include	"LHeader/LIGA_Physics.h"	

//Objects of static items
#include	"LHeader/LIGA_ResMan.h"					// Resource manager
#include	"LHeader/LIGA_Sky.h"					// Resource manager
#include	"LHeader/LIGA_Object.h"					// Object manager
#include	"LHeader/LIGA_QuadTree.h"				// QuadTree system
#include	"LHeader/LIGA_Terrain.h"				// Terrain engine
#include	"LHeader/LIGA_Trigger.h"
#include	"LHeader/LIGA_PP_Bloom.h"				// Post-Processor
#include	"LHeader/LIGA_HUD.h"					// HUD System
#include	"LHeader/LIGA_Partsys.h"					// Particleclass
#include	"LHeader/LIGA_SaveLoad.h"				// Extractor


#include	"LHeader/Player.h"					// PlayerClass
#include	"LHeader/PlayerShoot.h"
#include	"LHeader/PlayerCamera.h"

#include	"LHeader/LIGA_AppClass.h"			//Common application class
