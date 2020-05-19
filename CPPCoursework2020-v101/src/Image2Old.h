#pragma once

#pragma once

/**********************
STOP!!!!
You don't need to understand most of what is in this file.
You can probably even get away without using it - try SimpleImage instead.
I added a few methods that people hardly ever need into this class in case you want them.
Initialise this from a SimpleImage object if you wish (see the conversion constructor)
************************/

#include <memory>

#include "RawImage.h"

#include "SimpleImage.h"

class DrawingSurface;
class BaseEngine;
class CoordinateMapping;



/*
Image2 version - a more complex image than SimpleImage
*/
class Image2Old : public SimpleImage
{
public:
	// Conversion constructor - grab the image smart pointer from the source image
	Image2Old(const SimpleImage& srcImage)
		: SimpleImage(srcImage),
		m_iXSource(0), m_iYSource(0), m_iDrawWidth(0), m_iDrawHeight(0), m_iRotate90Degrees(0),
		m_iTransparencyColour(-1), m_iBrightnessPercentage(100), 
		m_iAlternatePixelColour(-1), m_iThirdPixelColour(-1), m_iFourthPixelColour(-1), 
		m_iMergeColour(-1), m_iMergePercentage(50), pRescaleSurface(nullptr)
	{
		// Initialise width and height to entire image
		m_iDrawWidth = theData->getWidth();
		m_iDrawHeight = theData->getHeight();
	}
	
	// Default constructor - assumes assignment will be performed later
	Image2Old()
		: m_iXSource(0), m_iYSource(0), m_iDrawWidth(0), m_iDrawHeight(0), m_iRotate90Degrees(0),
		m_iTransparencyColour(-1), m_iBrightnessPercentage(100),
		m_iAlternatePixelColour(-1), m_iThirdPixelColour(-1), m_iFourthPixelColour(-1),
		m_iMergeColour(-1), m_iMergePercentage(50), pRescaleSurface(nullptr)
	{
		// Initialise width and height to entire image
		m_iDrawWidth = 0;
		m_iDrawHeight = 0;
	}

	~Image2Old()
	{
		if (pRescaleSurface != nullptr)
		{
			SDL_FreeSurface(pRescaleSurface);
			pRescaleSurface = nullptr;
		}
	}

	// Assignment operator, so we can take the simpleimage object and set width and height correctly
	Image2Old& operator=(const SimpleImage& rhs)
	{
		if (&rhs != this)
		{
			theData = rhs.getTheData();
			m_iDrawWidth = theData->getWidth();
			m_iDrawHeight = theData->getHeight();
		}
		return *this;
	}



	// Set a specific colour to be the transparancy colour
	void setTransparencyColour(int colour)
	{
		m_iTransparencyColour = colour;
		// And update the alpha values for transparency in case we blit the surface later...
		if (m_iTransparencyColour == -1)
			getTheData()->setOpaque();
		else
			getTheData()->setTransparentColour(m_iTransparencyColour);
	}

	// Position of the transparency pixel - i.e. pixels matching this one should be transparent
	void setTransparencyPixel(int x, int y)
	{
		if ((x > -1) && (x < getWidth()) && (y > -1) && (y < getHeight()))
		{
			setTransparencyColour( theData->getRawPixelColour(x, y) );
		}
	}

	/* Specify the size of the image to draw */
	void setDrawSize(int width, int height)
	{
		m_iDrawWidth = width;
		m_iDrawHeight = height;
	}

	/* Specify the top-left of the drawing position within the image */
	void setDrawImagePosition(int x, int y)
	{
		m_iXSource = x;
		m_iYSource = y;
	}

	/* Set the brightness percentage - e.g. to wash-out an image */
	void setBrightness(int percentage) { m_iBrightnessPercentage = percentage; }

	/* These methods are used to set specific pixels to specific colours */
	void clearAlternativeColours() { m_iAlternatePixelColour = m_iThirdPixelColour = m_iFourthPixelColour = -1; }
	void colourAlternatePixels(int colour) { m_iAlternatePixelColour = colour; }
	void colourThirdPixels(int colour) { m_iThirdPixelColour = colour; }
	void colourFourthPixels(int colour) { m_iFourthPixelColour = colour; }

	// Merge all pixels with this specified colour
	void mergeWith(int colour) { m_iMergeColour = colour; }
	// Merge all pixels with background - i.e. create some kind of transparency
	void mergeWithBackground() { m_iMergeColour = -2; }
	// Don't merge pixels
	void noMerge() { m_iMergeColour = -1; }
	// Specify the percentage of the image to keep when merging, e.g. 50%
	void setMergePercentage(int percentage) { if (percentage < 0)percentage = 0; else if (percentage > 100) percentage = 100; m_iMergePercentage = percentage; }

	// Rotate image in multiples of 90 degrees.
	void setRotation(int increments) { m_iRotate90Degrees = increments; }

	/* Render the current image using the stored values of image size and offset, transparency colour, etc */
	void renderImageUsingSettings(
		DrawingSurface* pTarget, // Target surface to render to
		int iXTarget, int iYTarget // Coordinates in dest to start draw to
	) const;

	/*
	This is a simpler rendering, to a specified size reduction rather than working out the pixels to draw.
	This uses the stored values where possible rather than having to pass everything in each time.
	*/
	void renderImageReducedSize(BaseEngine* pEngine, DrawingSurface* pTarget, int iXTarget, int iYTarget, int iSizeReduction = 1) const
	{
		return renderImageReducedSize(pEngine, pTarget, iXTarget, iYTarget,
			m_iXSource, m_iYSource, m_iDrawWidth, m_iDrawHeight,
			iSizeReduction,
			m_iTransparencyColour);
	}

	/*
	This is a simpler rendering, to a specified size reduction rather than working out the pixels to draw.
	*/
	void renderImageReducedSize(BaseEngine* pEngine, DrawingSurface* pTarget, int iXTarget, int iYTarget,
		int iLeftInImage, int iTopInImage, int iWidthInImage, int iHeightInImage, int iSizeReduction = 1,
		int iTransparencyColour = -1) const;


	/*
	This render uses a blit operation.
	I had some problems making this work on some systems but it may be useful.
	iXDrawLocation, iYDrawLocation : location to draw to
	iTargetWidth, iTargetHeight : size of target rectangle
	iLeftInImage, iTopInImage : position in image
	iWidthInImage, iHeightInImage : size of source rectangle
	Transparency colour (i.e. alpha values) must have been set before calling this method if you need it. See setTransparencyColour() and setTransparencyPixel();
	*/
	void renderImageBlit(BaseEngine* pEngine, DrawingSurface* pTarget,
		int iXDrawLocation, int iYDrawLocation, int iTargetWidth, int iTargetHeight,
		int iLeftInImage, int iTopInImage, int iWidthInImage, int iHeightInImage);

	/*
	This render uses a blit operation.
	I had some problems making this work on some systems but it may be useful.
	iXDrawLocation, iYDrawLocation : location to draw to
	iTargetWidth, iTargetHeight : size of target rectangle
	Position and size in source image are pulled from member variables
	Transparency colour (i.e. alpha values) must have been set before calling this method if you need it. See setTransparencyColour() and setTransparencyPixel();
	*/
	void renderImageBlit(BaseEngine* pEngine, DrawingSurface* pTarget,
		int iXDrawLocation, int iYDrawLocation, int iTargetWidth, int iTargetHeight)
	{
		return renderImageBlit( pEngine, pTarget,
			iXDrawLocation, iYDrawLocation, iTargetWidth, iTargetHeight,
			m_iXSource, m_iYSource, m_iDrawWidth, m_iDrawHeight );
	}


protected:
	int m_iXSource, m_iYSource; // Coordinates in source to start draw from

	int m_iDrawWidth, m_iDrawHeight; // Size of the area to copy - in the SOURCE not destination - since it may be swapped in destination.

	// Setting iRotate90Degrees to 1 will rotate 90 degrees clockwise, to 2 will rotate 180 degrees and to 3 will rotate 270 degrees. Only values 0-3 have distinct meanings.
	int m_iRotate90Degrees; // Number of 90 degree rotations to apply
	int m_iTransparencyColour; 
	// If you set iBrightnessPercentage to other than 100 you can make the image darker or brighter (reduces or increases each of the RGB values). Note that each will be saturated at 255 though so going too bright can appear to change the colour.
	int m_iBrightnessPercentage; // Percentage brightness to use
	// If you set the iAlternatePixelColour to other than - 1 then every alternate pixel will be coloured to the NthPixel colour.
	int m_iAlternatePixelColour; // Pixel colour for each third pixel - set to -1 to not use
	// If you set the iThirdPixelColour to other than - 1 then every third pixel will be coloured to the NthPixel colour.
	int m_iThirdPixelColour; // Pixel colour for each third pixel - set to -1 to not use
	// If you set the iFourthPixelColour to other than - 1 then every third pixel will be coloured to the NthPixel colour.
	int m_iFourthPixelColour; // Pixel colour for each fourth pixel - set to -1 to not use
	// If you set iMergeColour to other than - 1 then any remaining pixel colours will be averaged with this one - e.g.to make it more grey ?
	int m_iMergeColour; // Merge colour - set to -1 to not use, otherwise this will average with current pixels

	int m_iMergePercentage;

	SDL_Surface* pRescaleSurface;
};




