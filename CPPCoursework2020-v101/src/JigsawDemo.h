#ifndef JIGSAW_H
#define JIGSAW_H


#include "BaseEngine.h"
#include "DragableObject.h"
#include "CoordinateMapping.h"
#include "DrawingFilters.h" 



class PieceFilter
{
	const int width = 70;
	const int height = 70;

public:
	PieceFilter(bool left, bool right, bool top, bool bottom)
	{
		filterData = new bool[width * height];
		for (int i = 0; i < width * height; i++)
			filterData[i] = false;
		for (int x = 0; x < 50; x++)
			for (int y = 0; y < 50; y++)
				filterData[x + y * width] = true;
		if (left) // Cut out a part on left hand side
		{
			// First the peg then the circle
			for (int x = 0; x < 10; x++)
				for (int y = 20; y < 30; y++)
					filterData[x + y * width] = false;
			for (int x = 0; x < 20; x++)
				for (int y = 5; y < 45; y++)
					if (((x - 5)*(x - 5) * 2 + (y - 25)*(y - 25) / 2) <= 100)
						filterData[x + y * width] = false;
		}
		if (right) // Cut out a part on right hand side
		{
			// First the peg then the circle
			for (int x = 50; x < 60; x++)
				for (int y = 20; y < 30; y++)
					filterData[x + y * width] = true;
			for (int x = 50; x < 70; x++)
				for (int y = 5; y < 45; y++)
					if (((x - 55)*(x - 55) * 2 + (y - 25)*(y - 25) / 2) <= 100)
						filterData[x + y * width] = true;
		}
		if (top) // Cut out a part on left hand side
		{
			// First the peg then the circle
			for (int y = 0; y < 10; y++)
				for (int x = 20; x < 30; x++)
					filterData[x + y * width] = false;
			for (int y = 0; y < 20; y++)
				for (int x = 5; x < 45; x++)
					if (((y - 5)*(y - 5) * 2 + (x - 25)*(x - 25) / 2) <= 100)
						filterData[x + y * width] = false;
		}
		if (bottom) // Cut out a part on right hand side
		{
			// First the peg then the circle
			for (int y = 50; y < 60; y++)
				for (int x = 20; x < 30; x++)
					filterData[x + y * width] = true;
			for (int y = 50; y < 70; y++)
				for (int x = 5; x < 45; x++)
					if (((y - 55)*(y - 55) * 2 + (x - 25)*(x - 25) / 2) <= 100)
						filterData[x + y * width] = true;
		}

		// Output the pattern to check it
		//for (int y = 0; y < 70; y++)
		//{
		//	for (int x = 0; x < 70; x++)
		//		std::cout << (filterData[x + y * 70] ? "X" : ".");
		//	std::cout << std::endl;
		//}
	}

	~PieceFilter()
	{
		delete[] filterData;
	}


	// Determine whether filter says we should draw the pixel
	bool shouldDraw(int x, int y) { return x < width && y < height && x >=0 && y>= 0 && filterData[x + y * width]; }

private:
	//int m_iWidth = 70;
	//int m_iHeight = 70;
	//int m_iBaseHeight = 50;
	//int m_iBaseWidth = 50;
	bool* filterData;
};



/*
This is a demo showing how to use the coordinate mapping objects in image objects
*/
class JigsawDemo : public BaseEngine
{
public:
	enum mode { NORMAL, CLOCKWISE, ANTICLOCKWISE };

	/**
	Constructor
	*/
	JigsawDemo() 
		: m_filterScaling(0, 0, this), m_filterTranslation(0, 0, &m_filterScaling)
		, middle(true, true, true, true)
		, topleft(false, true, false, true)
		, left(false, true, true, true)
		, bottomleft(false, true, true, false)
		, bottom(true, true, true, false)
		, bottomright(true, false, true, false)
		, right(true, false, true, true)
		, topright(true, false, false, true)
		, top(true, true, false, true)
		, m_iMode(NORMAL)
		, m_bDragging(false), m_iClickedX(0), m_iClickedY(0)
	{
		m_bBlockWaitingForEvents = true; // Don't keep constantly looping, instead wait for an event, e.g. click
		notifyTopObjectAboutMouseClick(true); // Tell framework to tell top object about clicks - ensures virtSingleItemMouseDown is called
	}

	// Quick code to save positions of all jigsaw pieces
	void savePositions();

	// Do any setup of back buffer prior to locking the screen buffer
	// Basically do the drawing of the background in here and it'll be copied to the screen for you as needed
	virtual void virtSetupBackgroundBuffer() override;

	// Initialise sets the filters for shift and zoom
	int virtInitialise() override;
		
	// Create any moving objects
	int virtInitialiseObjects() override;

	// Handle pressing of a key
	virtual void virtKeyDown(int iKeyCode) override;

	// This forces a screen redraw every pass through the loop.
	virtual void virtMainLoopDoBeforeUpdate() override;

	void virtKeyUp(int iKeyCode) override;

	/* We use this to support clicking on the edge for scrolling etc */
	void virtRealMouseDown(int iButton, int iX, int iY) override;

	/* We use this to keep a track of when the background is clicked, so we can drag to scroll */
	virtual void virtBackgroundMouseDown(int iButton, int iRealX, int iRealY, int iVirtualX, int iVirtualY) override;

	// Used to allow the background to scroll
	virtual void virtMouseMoved(int iX, int iY) override;
	// Used to allow the background to scroll
	virtual void virtMouseUp(int iButton, int iX, int iY) override;

	void virtMouseWheel(int x, int y, int which, int timestamp) override;

	// Draw anything else before the sctrings and objects
	virtual void virtPreDraw() override;

	// Draw anything else afterwards
	virtual void virtPostDraw() override;

	// Get the current mode
	mode getMode() { return m_iMode; }

	void zoomOut()
	{
		// We grab the position of the centre of the screen before the zoom
		int iOldCentreX = convertClickedToVirtualPixelXPosition(this->getWindowWidth() / 2);
		int iOldCentreY = convertClickedToVirtualPixelYPosition(this->getWindowHeight() / 2);
		m_filterScaling.compress();
		// Now we grab the position after the zoom
		int iNewCentreX = convertClickedToVirtualPixelXPosition(this->getWindowWidth() / 2);
		int iNewCentreY = convertClickedToVirtualPixelYPosition(this->getWindowHeight() / 2);
		m_filterTranslation.changeOffset(iNewCentreX - iOldCentreX, iNewCentreY - iOldCentreY);
		redrawDisplay(); // Force total redraw
	}

	void zoomIn()
	{
		// We grab the position of the centre of the screen before the zoom
		int iOldCentreX = convertClickedToVirtualPixelXPosition(this->getWindowWidth() / 2);
		int iOldCentreY = convertClickedToVirtualPixelYPosition(this->getWindowHeight() / 2);
		m_filterScaling.stretch();
		// Now we grab the position after the zoom
		int iNewCentreX = convertClickedToVirtualPixelXPosition(this->getWindowWidth() / 2);
		int iNewCentreY = convertClickedToVirtualPixelYPosition(this->getWindowHeight() / 2);
		m_filterTranslation.changeOffset(iNewCentreX - iOldCentreX, iNewCentreY - iOldCentreY);
		redrawDisplay(); // Force total redraw
	}

protected:
	// Note that static members are only initialised once, so this only creates the one object of each type, and keeps a pointer to it
	FilterPointsScaling m_filterScaling;
	FilterPointsTranslation m_filterTranslation;

	mode m_iMode;

	bool m_bDragging;
	int m_iClickedX;
	int m_iClickedY;

	PieceFilter middle;
	PieceFilter topleft;
	PieceFilter left;
	PieceFilter bottomleft;
	PieceFilter bottom;
	PieceFilter bottomright;
	PieceFilter right;
	PieceFilter topright;
	PieceFilter top;
};

#endif
