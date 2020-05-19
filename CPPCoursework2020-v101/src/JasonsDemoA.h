#pragma once

// My final version of the demo you are asked to walk through to learn the basics.

#include "BaseEngine.h"
#include "JasonsTileManager.h"

class JasonsDemoA :
	public BaseEngine
{
public:
	JasonsDemoA();
	~JasonsDemoA();
	void virtSetupBackgroundBuffer();
	void virtMouseDown(int iButton, int iX, int iY);
	void virtKeyDown(int iKeyCode);

protected:
	JasonsTileManager tm;
public:
	int virtInitialiseObjects();
};

