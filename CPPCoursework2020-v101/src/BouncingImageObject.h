#pragma once

// A simple example which illustrates how to use a SimpleImage to draw a moving object.
// This was designed to try to keep it simple so you could see how to do the drawing yourself in your own objects.

#include "BaseEngine.h"

#include "SimpleDemo.h"

#include "ImageManager.h"
#include "TileManager.h"
#include "DisplayableObject.h"
#include "BouncingObject.h"

/*
This draws and moves an image on the screen.
Look at the lines labelled LOOK HERE
*/
class BouncingImageObject : public BouncingObject
{
public:
	// Constructor has to set tell the image what URL to load, as well as telling the base class to initialise itself.
	BouncingImageObject(BaseEngine* pEngine, std::string strURL)
		: BouncingObject(pEngine)
		, image(pEngine->loadImage(strURL, false/*don't keep it loaded*/))  // LOOK HERE
	{
		// The following line is entirely optional!
		image = image.shrinkBy(4); // This will divide the image size by 4.
		// Note: this actually creates a new SimpleImage of smaller size, and then the assignment operator here copies the new image into your image object

		// Nothing to do since base class will do it all!
		// Note that the width and height are hard-coded in the base class - but you can fix that when you create your own class.
	}

	// Draw the shape - by asking the image to draw itself to the foreground at the specified position.
	virtual void virtDraw() override
	{
		if (isVisible())
		{  // LOOK HERE (at the line below)
			image.renderImageWithMask(getEngine()->getForegroundSurface(), 0, 0, m_iCurrentScreenX + m_iStartDrawPosX, m_iCurrentScreenY + m_iStartDrawPosY, m_iDrawWidth, m_iDrawHeight);
		}
	}

private:
	// Note: image is an OBJECT, not a POINTER. This is Composition in C++ - the SimpleImage object is actually a part of BouncingImageObject
	// Automatically destroyed when the BouncingImageObject it is a part of is destroyed.
	SimpleImage image;   // LOOK HERE
};
