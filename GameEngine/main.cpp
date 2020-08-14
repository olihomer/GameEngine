#include "AppWindow.h"
#include "GraphicsEngine.h"
#include "InputSystem.h"
#include <iostream>

int main()
{
	std::exception ex;

	try
	{
		GraphicsEngine::create();
		InputSystem::create();
	}
	catch (std::exception &ex)
	{
		return -1;
		std::cout << ex.what();
	}

	try
	{
		AppWindow app;
		OutputDebugString("Entering Main Loop\n");
		while (app.isRun());
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what();
		GraphicsEngine::release();
		InputSystem::release();
		return -1;
	}
	
	GraphicsEngine::release();
	InputSystem::release();
	return 0;
}