#pragma once
#include "Point.h"

class InputListener
{
public:
	InputListener()
	{}
	~InputListener()
	{}
	void test();
	//KEYBOARD pure virtual callback functions

	virtual void onKeyDown(int key) = 0;
	virtual void onKeyUp(int key) = 0;
	virtual void onMouseMove(Point point) = 0;
	virtual void onLeftMouseDown(Point point) = 0;
	virtual void onRightMouseDown(Point point) = 0;
	virtual void onLeftMouseUp(Point point) = 0;
	virtual void onRightMouseUp(Point point) = 0;

};