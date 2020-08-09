#include "AppWindow.h"
#include "GraphicsEngine.h"
#include "InputSystem.h"

int main()
{
	try
	{
		GraphicsEngine::create();
		InputSystem::create();
	}
	catch (...) { return -1; }
	
	AppWindow app;

	if (app.init())
	{
		OutputDebugString("Entering Main Loop\n");
		//main loop
		
		while (app.isRun())
		{
			app.broadcast();
		}
	}

	GraphicsEngine::release();
	InputSystem::release();

	return 0;
}