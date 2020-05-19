#pragma once
#include "DisplayableObject.h"

// My final version of the object created in the demo you are asked to walk through to learn the basics.

class JasonsObjectA :
	public DisplayableObject
{
public:
	JasonsObjectA(BaseEngine* pEngine);
	~JasonsObjectA();

	void virtDraw() override;
	void virtDoUpdate(int iCurrentTime) override;
};

