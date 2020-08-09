#pragma once
#include <Windows.h>

class Window
{
public:
	Window();
	~Window();
	bool isRun();
	RECT getClientWindowRect();

	//EVENTS
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();

	void broadcast();
protected:
	HWND m_hwnd=0;
	bool m_is_run=false;
	bool m_is_init = false;
};

