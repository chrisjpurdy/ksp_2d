#include "header.h"

#define SHUFFLE_PIECES

// This is a basic implementation, without fancy stuff

#include "BaseEngine.h"

#include "JigsawDemo.h"
#include "JigsawPiece.h"

#include "ImageManager.h"

#include "TileManager.h"

#include "DisplayableObject.h"

#include "ImageObject.h"
#include "MappableImageObject.h"

#include <algorithm> 





/*
Initialise things for my specific game/program
*/
int JigsawDemo::virtInitialise()
{
	// These only need to be called once, so I didn't want to put them in functions which are called multiple times, even though I now need to override this method too
	getBackgroundSurface()->setDrawPointsFilter(&m_filterTranslation);
	getForegroundSurface()->setDrawPointsFilter(&m_filterTranslation);

	// Call base class version
	return BaseEngine::virtInitialise();
}


/*
Do any setup of back buffer prior to locking the screen buffer
Basically do the drawing of the background in here and it'll be copied to the screen for you as needed
*/
void JigsawDemo::virtSetupBackgroundBuffer()
{
	fillBackground(0x404040);
}


#include <cstdio>

void JigsawDemo::savePositions()
{
	FILE* fileSave = fopen( "jigsaw_simple_save_state.txt", "w");
	if (fileSave != nullptr)
	{
		int count = 0;
		this->applyToAll([&](DisplayableObject* ob) { if ( ob != nullptr ) ((JigsawPiece*)ob)->write(fileSave,count++); });
		fclose(fileSave);
	}
}

/*
In here you need to create any movable objects that you wish to use.
Sub-classes need to implement this function.
*/
int JigsawDemo::virtInitialiseObjects()
{
	// Destroy any existing objects
	destroyOldObjects(true);

	std::string strImageURL = "images/mountain-4635428_480.jpg";

	FILE* fileLoad = fopen("jigsaw_simple_save_state.txt", "r");

	SimpleImage image = ImageManager::loadImage(strImageURL, true);
	int xMax = image.getWidth() / 50;
	int yMax = image.getHeight() / 50;

	// Add a dragable shape per jigsaw piece
	createObjectArray(xMax * yMax +1);
	storeObjectInArray(xMax * yMax, nullptr);

	int* positions = new int[xMax * yMax];
	for (int i = 0; i < (xMax * yMax); i++)
		positions[i] = i;

	// Comment the following out to not shuffle the pieces initially...
#ifdef SHUFFLE_PIECES
	for (int i = 0; i < (xMax * yMax); i++)
	{
		int other = rand() % (xMax * yMax);
		std::swap(positions[i], positions[other]);
	}
#endif
	//for (int i = 0; i < (xMax * yMax); i++)
	//	std::cout << positions[i] << " ";
	//std::cout << std::endl;

	int iPiece = 0;
	for (int x = 0; x < xMax; x++)
	{
		for (int y = 0; y < yMax; y++)
		{
			//int iPosX = 25 * (rand() % 120);
			//int iPosY = 25 * (rand() % 96);
			int iRandPos = positions[x + y * xMax];
			int iPosX = 75 * (iRandPos % xMax);
			int iPosY = 75 * (iRandPos / xMax);
			//int iPosX = x * 75;
			//int iPosY = y * 75;
			JigsawPiece* ob = new JigsawPiece(fileLoad, iPosX, iPosY, this/*Engine*/, strImageURL, x * 50, y * 50 );
			storeObjectInArray(iPiece++, ob);
			if ( ob->getXInImage()/50 == 0)
			{
				if (ob->getYInImage() / 50 == 0)
					ob->setFilter(&topleft);
				else if (ob->getYInImage() / 50 == (yMax-1))
					ob->setFilter(&bottomleft);
				else
					ob->setFilter(&left);
			}
			else if (ob->getXInImage()/50 == (xMax-1))
			{
				if (ob->getYInImage() / 50 == 0)
					ob->setFilter(&topright);
				else if (ob->getYInImage() / 50 == (yMax - 1))
					ob->setFilter(&bottomright);
				else
					ob->setFilter(&right);
			}
			else 
			{
				if (ob->getYInImage() / 50 == 0)
					ob->setFilter(&top);
				else if (ob->getYInImage() / 50 == (yMax - 1))
					ob->setFilter(&bottom);
				else
					ob->setFilter(&middle);
			}
		}
	}

	// The positions may have been set when loading, so any which were loaded should be re-linked
	applyToAll([&](DisplayableObject* ob) { if (ob != nullptr) ((JigsawPiece*)ob)->checkForLinksWithOthers(); });

	if (fileLoad)
		fclose(fileLoad);

	// Remove the positions array that we created
	delete [] positions;

	return 0;
}




// This forces a screen redraw every pass through the loop.
void JigsawDemo::virtMainLoopDoBeforeUpdate()
{
	int iOffset = getModifiedTime() / 10;
}


/*
Handle any key presses here.
Note that the objects themselves (e.g. player) may also check whether a key is currently pressed
*/
void JigsawDemo::virtKeyDown(int iKeyCode)
{
	switch (iKeyCode)
	{
	case SDLK_ESCAPE: // End program when escape is pressed
		setExitWithCode(0);
		break;
	case SDLK_LEFT:
		m_filterTranslation.changeOffset(10, 0);
		redrawDisplay();
		break;
	case SDLK_RIGHT:
		m_filterTranslation.changeOffset(-10, 0);
		redrawDisplay();
		break;
	case SDLK_UP:
		m_filterTranslation.changeOffset(0, 10);
		redrawDisplay();
		break;
	case SDLK_DOWN:
		m_filterTranslation.changeOffset(0, -10);
		redrawDisplay();
		break;
	case SDLK_SPACE:
		m_filterTranslation.setOffset(0, 0);
		redrawDisplay();
		break;
	default:
		redrawDisplay();
		break;
	}
}



/* Override this to handle a key being released.
Note that the fact that the key has been released will have already been stored.
*/
void JigsawDemo::virtKeyUp(int iKeyCode)
{
	redrawDisplay();
}



// Override to handle a mouse press - when mouse is pressed for a refresh before the drag
void JigsawDemo::virtRealMouseDown(int iButton, int iX, int iY)
{
	//std::cout << "virtRealMouseDown" << std::endl;

	int width = getWindowWidth();
	int height = getWindowHeight();
	//std::cout << "Click " << iButton << " at " << iX << "," << iY << " size " << width << "," << height << std::endl;

	switch (iButton)
	{
	case 1: // Left
		if (iX < 50)
		{
			if (iY < 50)
			{ // top left
				if (m_iMode == ANTICLOCKWISE)
					m_iMode = NORMAL;
				else
					m_iMode = ANTICLOCKWISE;
				redrawDisplay();
			}
			else if (iY > height - 50)
			{ // bottom left
				zoomOut();
				redrawDisplay();
			}
			else if (iX < 25)
			{ // far left
			//	m_filterTranslation.changeOffset(-100, 0);
				redrawDisplay();
			}
			else // Normal slow scroll
			{ // not so far left
			//	m_filterTranslation.changeOffset(-50, 0);
				redrawDisplay();
			}
		}
		if (iX > width-50)
		{
			if (iY < 50)
			{ // top right
				if (m_iMode == CLOCKWISE)
					m_iMode = NORMAL;
				else
					m_iMode = CLOCKWISE;
				redrawDisplay();
			}
			else if (iY > height - 50)
			{ // bottom right
				zoomIn();
				redrawDisplay();
			}
			else if (iX > width - 25)
			{ // far right
			//	m_filterTranslation.changeOffset(100, 0);
				redrawDisplay();
			}
			else // Normal slow scroll
			{ // not so far right
			//	m_filterTranslation.changeOffset(50, 0);
				redrawDisplay();
			}
		}
		else // Not the far left or right
		{
			// Centre
			if ((iX > width / 2 - 25) && (iX < width / 2 + 25) && (iY > height / 2 - 25) && (iY < height / 2 + 25))
			{
				savePositions();
				// Also check for extra links while we are at it...
				applyToAll([&](DisplayableObject* ob) { if (ob != nullptr) ((JigsawPiece*)ob)->checkForLinksWithOthers(); });
				//m_filterTranslation.setOffset(0, 0);
				m_iMode = NORMAL;
				redrawDisplay();// m_filterTranslation.changeOffset();
			}
			else
			{
				if (iY < 25)
				{
				//	m_filterTranslation.changeOffset(0, -100);
					redrawDisplay();
				}
				else if (iY < 50)
				{
				//	m_filterTranslation.changeOffset(0, -50);
					redrawDisplay();
				}
				else if (iY > height - 25)
				{
				//	m_filterTranslation.changeOffset(0, 100);
					redrawDisplay();
				}
				else if (iY > height - 50)
				{
				//	m_filterTranslation.changeOffset(0, 50);
					redrawDisplay();
				}
			}
		}
			// Nothing because we are going to click to drag objects anyway
		break;
	case 2: // Middle
			//zoomOut();
		break;
	case 3: // Right
			//zoomIn();
		break;
	}
	// Redraw the background
	redrawDisplay(); // Force total redraw
}



void JigsawDemo::virtMouseWheel(int x, int y, int which, int timestamp)
{
	int iOldCentreX = convertClickedToVirtualPixelXPosition(this->getWindowWidth() / 2);
	int iOldCentreY = convertClickedToVirtualPixelYPosition(this->getWindowHeight() / 2);

	if (y < 0)
		zoomOut();
	else if (y > 0)
		zoomIn();

	int iNewCentreX = convertClickedToVirtualPixelXPosition(this->getWindowWidth() / 2);
	int iNewCentreY = convertClickedToVirtualPixelYPosition(this->getWindowHeight() / 2);

	//float fNewZoomX = m_filterScaling.getZoomX();
	//float fNewZoomY = m_filterScaling.getZoomY();
	//fNewZoomX = fNewZoomY = 1.0f;

	//std::cout << "Zoom from x " << std::to_string(iOldCentreX) << " to " << std::to_string(iNewCentreX)
	//	<< ", y " << std::to_string(iOldCentreY) << " to " << std::to_string(iNewCentreY)
	//	<< ", shift by " << std::to_string(iNewCentreX - iOldCentreX) << "," << std::to_string(iNewCentreY - iOldCentreY)
	//	<< std::endl;

	// Apply a translation to offset so it appears to have zoomed on the centre.
	m_filterTranslation.changeOffset(iNewCentreX - iOldCentreX, iNewCentreY - iOldCentreY);

	// Redraw the background
	redrawDisplay(); // Force total redraw
}


// Draw anything else before the sctrings and objects
void JigsawDemo::virtPreDraw()
{
	// Base version does nothing
}


// Draw anything else afterwards
void JigsawDemo::virtPostDraw()
{
	// Base version does nothing
	int width = getWindowWidth();
	int height = getWindowHeight();
	//for (int i = 0; i < height; i += 2)
	//{
	//	getForegroundSurface()->rawSetPixel(50, i,0xff0000);
	//	getForegroundSurface()->rawSetPixel(width-50, i, 0xff0000);
	//}
	//for (int i = 0; i < width; i += 2)
	//{
	//	getForegroundSurface()->rawSetPixel(i,50, 0xff0000);
	//	getForegroundSurface()->rawSetPixel(i, height - 50, 0xff0000);
	//}
	for (int i = width/2-25; i < width/2+25; i += 2)
	{
		getForegroundSurface()->rawSetPixel(i, height / 2 - 25, 0xffff00);
		getForegroundSurface()->rawSetPixel(i, height / 2 + 25, 0xffff00);
	}
	for (int i = height / 2 - 25; i < height / 2 + 25; i += 2)
	{
		getForegroundSurface()->rawSetPixel(width / 2 - 25, i, 0xffff00);
		getForegroundSurface()->rawSetPixel(width / 2 + 25, i, 0xffff00);
	}
	if (m_iMode == ANTICLOCKWISE)
	{
		getForegroundSurface()->rawDrawOval(5, 5, 45, 45, 0x000000);
		getForegroundSurface()->rawDrawOval(15, 15, 35, 35, 0xffff00);
	}
	if (m_iMode == CLOCKWISE)
	{
		getForegroundSurface()->rawDrawOval(width - 5, 5, width - 45, 45, 0x000000);
		getForegroundSurface()->rawDrawOval(width - 15, 15, width - 35, 35, 0xffff00);
	}
	getForegroundSurface()->rawDrawOval(5, height - 5, 45, height - 45, 0x000000);
	getForegroundSurface()->rawDrawOval(20, height-20, 30, height-30, 0xffffff);
	getForegroundSurface()->rawDrawOval(width - 5, height - 5, width - 45, height - 45, 0x000000);
	getForegroundSurface()->rawDrawOval(width - 10, height-10, width - 40, height-40, 0xffffff);
}


/* We use this to keep a track of when the background is clicked, so we can drag to scroll */
void JigsawDemo::virtBackgroundMouseDown(int iButton, int iRealX, int iRealY, int iVirtualX, int iVirtualY)
{
	m_bDragging = true;
	m_iClickedX = iVirtualX;
	m_iClickedY = iVirtualY;
}


// Used to allow the background to scroll
void JigsawDemo::virtMouseMoved(int iX, int iY)
{
	if (m_bDragging)
	{
		m_filterTranslation.changeOffset(iX - m_iClickedX, iY - m_iClickedY);
		//m_iClickedX = iX;
		//m_iClickedY = iY;
		redrawDisplay();
	}
}


// Used to allow the background to scroll
void JigsawDemo::virtMouseUp(int iButton, int iX, int iY)
{
	if (m_bDragging)
	{
		m_filterTranslation.changeOffset(iX - m_iClickedX, iY - m_iClickedY);
		m_bDragging = false;
		redrawDisplay();
	}
}

