#pragma once
#include "header.h"

#include "DisplayableObject.h"
#include "BaseEngine.h"

class DragableBaseClassObject : public DisplayableObject
{
public:
	// Constructors
	DragableBaseClassObject(BaseEngine* pEngine, int iWidth = 0, int iHeight = 0, bool useTopLeftFor00 = true)
		: DisplayableObject(pEngine, iWidth, iHeight, useTopLeftFor00), 
		m_bIsBeingDragged(false)
	{
	}

	DragableBaseClassObject(int xStart, int yStart, BaseEngine* pEngine, int iWidth, int iHeight, bool useTopLeftFor00 = true)
		: DisplayableObject(xStart, yStart, pEngine, iWidth, iHeight, useTopLeftFor00), m_bIsBeingDragged(false)
	{
	}

	// This is called instead of redraw directly so you could optionally redraw rectangle instead of screen.
	// Implement this to redrawRectangle or redrawScreen
	virtual void doDragRedraw()
	{
		redrawDisplay(); // It moved so do a redraw!
	}

	// Call this from mouse-down if you want to start a drag - it will check the bounds first through.
	virtual void checkStartDrag(int iX, int iY)
	{
		//std::cout << "checkStartDrag " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		if (virtIsPositionWithinObject(iX, iY))
		{
			// std::cout << "Drag started..." << std::endl;
			m_bIsBeingDragged = true;
			m_iInitialClickedDragX = iX;
			m_iInitialClickedDragY = iY;
			m_iPositionAtStartOfDragX = m_iCurrentScreenX;
			m_iPositionAtStartOfDragY = m_iCurrentScreenY;
			doDragRedraw();
		}
	}

	// Call from mouse moved if you want to 
	virtual void checkDoDrag(int iX, int iY)
	{
		//std::cout << "Mouse move " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		if (m_bIsBeingDragged)
		{
			doDragRedraw(); // In case it is doing a rectangle we need to include the old position too before moving it
			m_iCurrentScreenX = m_iPositionAtStartOfDragX + iX - m_iInitialClickedDragX;
			m_iCurrentScreenY = m_iPositionAtStartOfDragY + iY - m_iInitialClickedDragY;
			doDragRedraw(); // It moved so do a redraw!
		}
	}

	// Call from mouse up to complete the drag if you wish
	virtual void checkFinishDrag(int iX, int iY)
	{
		if (m_bIsBeingDragged)
		{
			//std::cout << "checkFinishDrag " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
			m_bIsBeingDragged = false;
			doDragRedraw();
		}
	}

protected:
	bool m_bIsBeingDragged;
	int m_iPositionAtStartOfDragX, m_iPositionAtStartOfDragY;
	int m_iInitialClickedDragX, m_iInitialClickedDragY;
};
