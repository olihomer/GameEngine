#include "AppWindow.h"



int main()
{
	OutputDebugString("Entering Main\n");
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

	return 0;
}