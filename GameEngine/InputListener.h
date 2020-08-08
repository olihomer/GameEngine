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
	virtual void onMouseMove(const Point& point) = 0;
	virtual void onLeftMouseDown(const Point& point) = 0;
	virtual void onRightMouseDown(const Point& point) = 0;
	virtual void onLeftMouseUp(const Point& point) = 0;
	virtual void onRightMouseUp(const Point& point) = 0;

};