#include "LHeader/LIGA_Setup.h"

/*
	3D Game engine. Just for fun, use it as you wish.
	Not refactored, very old and not clean code at all.
*/

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	App->Initialize(hInst, false, "LITHIUM ENGINE DEVELOPER");
	App->Run();
	return 0;
}
