#pragma once

#include "DisplayableObject.h"
#include "BaseEngine.h"
#include "DragableBaseClassObject.h"

class DragableObject : public DragableBaseClassObject
{
public:
	// Constructors
	DragableObject(BaseEngine* pEngine, int iWidth = 0, int iHeight = 0, bool useTopLeftFor00 = true, bool bVisible = true )
		: DragableBaseClassObject(pEngine, iWidth, iHeight, useTopLeftFor00)
	{
		pEngine->notifyObjectsAboutMouse(true); // Tell BaseEngine to send us the mouse events because we use them
		setVisible(bVisible);
	}

	DragableObject(int xStart, int yStart, BaseEngine* pEngine, int iWidth, int iHeight, bool useTopLeftFor00 = true, bool bVisible = true)
		: DragableBaseClassObject(xStart, yStart, pEngine, iWidth, iHeight, useTopLeftFor00)
	{
		pEngine->notifyObjectsAboutMouse(true); // Tell BaseEngine to send us the mouse events because we use them
		setVisible(bVisible);
	}

	/* Override if necessary - Mouse event - mouse button pressed. Must call notifyObjectsAboutMouse to tell base engine to notify objects, or this will not be called */
	virtual void virtMouseDown(int iButton, int iX, int iY)
	{
		//std::cout << "Mouse down " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		checkStartDrag(iX, iY);
	}

	/* Override if necessary - Mouse event - mouse was moved. Must call notifyObjectsAboutMouse to tell base engine to notify objects (see constructor), or this method will not be called */
	virtual void virtMouseMoved(int iX, int iY)
	{
		//std::cout << "Mouse move " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		checkDoDrag(iX, iY);
	}

	/* Override if necessary - Mouse event - mouse button released. Must call notifyObjectsAboutMouse to tell base engine to notify objects, or this will not be called */
	virtual void virtMouseUp(int iButton, int iX, int iY)
	{
		//std::cout << "Mouse up " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		checkFinishDrag(iX, iY);
		redrawDisplay(); // Force full redraw of screen
	}
};
