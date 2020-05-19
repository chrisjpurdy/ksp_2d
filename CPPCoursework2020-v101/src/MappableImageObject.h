#pragma once

#include "DragableImageObject.h"

class MappableImageObject : public DragableImageObject
{
public:
	// Constructors
	MappableImageObject(CoordinateMapping* pMapping, BaseEngine* pEngine, std::string strURL, bool useTopLeftFor00 = true, bool bVisible = true)
		: DragableImageObject(pEngine, strURL, useTopLeftFor00, bVisible)
		, m_pMapping(pMapping)
	{

	}

	MappableImageObject(CoordinateMapping* pMapping, int xStart, int yStart, BaseEngine* pEngine, std::string strURL, bool useTopLeftFor00 = true, bool bVisible = true)
		: DragableImageObject(xStart, yStart, pEngine, strURL, useTopLeftFor00, bVisible)
		, m_pMapping(pMapping)
	{
	}

	// Draw the object - override to implement the actual drawing of the correct object
	virtual void virtDraw()
	{
		if (isVisible())
		{
			image.renderImageApplyingMapping(getEngine(), getEngine()->getForegroundSurface(),
				m_iCurrentScreenX + m_iStartDrawPosX, m_iCurrentScreenY + m_iStartDrawPosY, m_iDrawWidth, m_iDrawHeight,
				m_pMapping);
		}
	}

	// Set the transparency colour
	void setTransparencyColour(int colour) { image.setTransparencyColour(colour); }

	// Retrieve the colour of a pixel - primarily used so we can get pixel 0,0 to set the transparency
	int getPixelColour(int x, int y) { return image.getPixelColour(x, y); }

	// Return the width of the loaded image
	int getImageWidth() { return image.getWidth(); }

	// Return the height of the loaded image
	int getImageHeight() { return image.getHeight(); }

protected:
	CoordinateMapping* m_pMapping;
};
