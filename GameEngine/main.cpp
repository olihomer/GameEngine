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

	try
	{
		AppWindow app;
		OutputDebugString("Entering Main Loop\n");
		while (app.isRun())
		{
			app.broadcast();
		}
	}
	catch (...)
	{
		GraphicsEngine::release();
		InputSystem::release();
		return -1;
	}
	
	GraphicsEngine::release();
	InputSystem::release();
	return 0;
}