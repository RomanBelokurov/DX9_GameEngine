#include "Engine/AppClass.h"


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	AppClass* myGame;
	bool result;

	myGame = new AppClass;
	
	if (!myGame)
		return 0;

	// Initialize and run the system object.
	if(myGame->Initialize())
	{
		myGame->Run();
	}

	// Shutdown and release the system object.
	myGame->Shutdown();
	delete myGame;
	myGame = 0;
	return 0;
}
