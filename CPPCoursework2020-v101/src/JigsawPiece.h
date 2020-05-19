#pragma once

#include "JigsawDemo.h"
#include "CoordinateMapping.h"
#include "DrawingFilters.h" 
#include "DragableImageObject.h"


class JigsawPiece : public DragableImageObject, public CoordinateMapping
{
public:
	JigsawDemo* m_pJigsaw;

	// Constructors
	JigsawPiece(FILE* fileLoad, int xStart, int yStart, JigsawDemo* pEngine,
		std::string strURL, int iXInImage, int iYInImage )
		: DragableImageObject(xStart, yStart, pEngine, strURL, true/*topleft*/, true/*visible*/)
		, m_iXInImage(iXInImage)
		, m_iYInImage(iYInImage)
		, m_iWidthInImage(70)
		, m_iHeightInImage(70)
		, m_pFilter(nullptr)
		, m_pJigsaw(pEngine)
	{
		m_iDrawWidth = m_iDrawHeight = 70;
		m_iCurrentScreenX = xStart;
		m_iCurrentScreenY = yStart;
		// Random rotation
		m_iRotation = 0; // ::rand() % 4;
		if (fileLoad != nullptr)
		{
			char buffer[1014];
			char* str = fgets(buffer, 1023, fileLoad);
			if (str != nullptr)
			{
				str[5] = 0; // 5 big - 5 is the x
				int count = atoi(str);
				str[13] = 0; // 8 big - 13 should be the y
				int iX = atoi(str + 6);
				str[21] = 0; // 8 big - 21 should be r
				int iY = atoi(str + 14);
				str[24] = 0; // 3 big - 24 should be c
				int iR = atoi(str + 22);
				str[32] = 0; // 8 big - 32 should be r
				int iCol = atoi(str + 25);
				int iRow = atoi(str + 33);
				//std::cout << "x " << iX << " y " << iY << " rot " << iR << " row " << iRow << " col " << iColumn << std::endl;
				m_iCurrentScreenX = iX;
				m_iCurrentScreenY = iY;
				m_iRotation = iR;
				m_iXInImage = iCol;
				m_iYInImage = iRow;
			}
		}
	}

	void write(FILE* fileSave, int i)
	{
		if (fileSave != nullptr)
		{
			fprintf(fileSave, "%05dx%07dy%07dr%02dc%07dr%07d\r\n", i,
				m_iCurrentScreenX, m_iCurrentScreenY, m_iRotation,
				m_iXInImage, m_iYInImage);
		}
	}

	// Draw the object - override to implement the actual drawing of the correct object
	virtual void virtDraw()
	{
		if (isVisible())
		{
			image.renderImageApplyingMapping(getEngine(), getEngine()->getForegroundSurface(),
				m_iCurrentScreenX + m_iStartDrawPosX, m_iCurrentScreenY + m_iStartDrawPosY,
				m_iDrawWidth, m_iDrawHeight,
				this/*CoordinateMapping*/);
			//std::cout << "width " << m_iDrawWidth << " height " << m_iDrawHeight << std::endl;
		}
	}

	// Set the transparency colour
	void setTransparencyColour(int colour) { image.setTransparencyColour(colour); }

	// Retrieve the colour of a pixel - primarily used so we can get pixel 0,0 to set the transparency
	int getPixelColour(int x, int y) { return image.getPixelColour(x, y); }

	// Return the width of the loaded image
	int getImageWidth() { return m_iWidthInImage; }

	// Return the height of the loaded image
	int getImageHeight() { return m_iHeightInImage; }

	// This is used to find the correct part of the image for the jigsaw piece that we are
	// It also rotates the pixels in the image if required.
	bool mapCoordinates(double& x, double& y, const SimpleImage& image)
	{
		double xin = x, yin = y;

		switch (m_iRotation % 4)
		{
		case 0:
			x = xin;
			y = yin;
			break;
		case 1:
			x = yin;
			y = 70 - xin;
			break;
		case 2:
			x = 70 - xin;
			y = 70 - yin;
			break;
		case 3:
			x = 70 - yin;
			y = xin;
			break;
		}

		// Ask filter whether to draw the pixel - note that it rounds off the x and y coords
		if (m_pFilter && !m_pFilter->shouldDraw( (int)(x+0.5), (int)(y+0.5)))
			return false; // Do not draw this pixel

		x += m_iXInImage;
		y += m_iYInImage;
		return true;
	}

	// Mouse buttom was clicked on THIS object as top object, so we should handle it now...
	virtual bool virtSingleItemMouseDown(int iButton, int iX, int iY) override
	{
		if (virtIsPositionWithinObject(iX, iY))
		{
			if ( iButton == SDL_BUTTON_LEFT )
			{
				getEngine()->moveToLast(this);// Bring this to the front

				switch (m_pJigsaw->getMode())
				{
				case JigsawDemo::NORMAL:
					redrawRectangle(); // Force refresh of this part of screen then drag this item 
					checkStartDrag(iX, iY);
					break;
				case JigsawDemo::CLOCKWISE:
					m_iRotation++;
					if (m_iRotation > 3)
						m_iRotation = 0;
					redrawRectangle(); // Force refresh of this part of screen
					break;
				case JigsawDemo::ANTICLOCKWISE:
					m_iRotation--;
					if (m_iRotation < 0)
						m_iRotation = 3;
					redrawRectangle(); // Force refresh of this part of screen
					break;
				}
			}
			else
			{
				m_iRotation++;
				redrawRectangle(); // Force refresh of this part of screen
			}
			return true;
		}
		return false;
	}

	/* Override if necessary - Mouse event - mouse button pressed. Must call notifyObjectsAboutMouse to tell base engine to notify objects, or this will not be called */
	virtual void virtMouseDown(int iButton, int iX, int iY)
	{
		// We record the start positions of all objects when mouse pressed, even if not the dragged object, so they can move with a dragged object
		m_iPositionAtStartOfDragX = m_iCurrentScreenX;
		m_iPositionAtStartOfDragY = m_iCurrentScreenY;

		// Do nothing because we handle this in virtSingleItemMouseDown so that only the top item is moved
		//std::cout << "Mouse down " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		//checkStartDrag(iX, iY);
	}

	virtual void recursiveDrag(int iXOffset, int iYOffset)
	{
		if (m_bMoved) return; // Already done this one
		m_bMoved = true;
		doDragRedraw(); // In case it is doing a rectangle we need to include the old position too before moving it
		m_iCurrentScreenX = m_iPositionAtStartOfDragX + iXOffset;
		m_iCurrentScreenY = m_iPositionAtStartOfDragY + iYOffset;
		doDragRedraw(); // In case it is doing a rectangle we need to include the old position too before moving it
		if (toLeft) toLeft->recursiveDrag(iXOffset, iYOffset);
		if (toRight) toRight->recursiveDrag(iXOffset, iYOffset);
		if (toTop) toTop->recursiveDrag(iXOffset, iYOffset);
		if (toBottom) toBottom->recursiveDrag(iXOffset, iYOffset);
	}

	/* Override if necessary - Mouse event - mouse was moved. Must call notifyObjectsAboutMouse to tell base engine to notify objects (see constructor), or this method will not be called */
	virtual void virtMouseMoved(int iX, int iY)
	{
		//std::cout << "Mouse move " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		//checkDoDrag(iX, iY);
		//std::cout << "Mouse move " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		if (m_bIsBeingDragged) // Have to do it here so we can pick up the connected ones too!
		{
			doDragRedraw(); // In case it is doing a rectangle we need to include the old position too before moving it
			int iXOffset = iX - m_iInitialClickedDragX;
			int iYOffset = iY - m_iInitialClickedDragY;
			recursiveDrag(iXOffset, iYOffset);
			m_iCurrentScreenX = m_iPositionAtStartOfDragX + iXOffset;
			m_iCurrentScreenY = m_iPositionAtStartOfDragY + iYOffset;
			//doDragRedraw(); // It moved so do a redraw!
			// Now recursively move the others too
			m_pJigsaw->applyToAll([&](DisplayableObject* ob) { if (ob != nullptr) ((JigsawPiece*)ob)->m_bMoved=false; });
			recursiveDrag(iXOffset, iYOffset);
		}
	}


	// After loading or moving a piece we need to see whether its location means it should be linked
	// Note: links are not persisted, hence the need to do it on loading too
	void checkForLinksWithOthers()
	{
		//std::cout << "check for link left" << std::endl;
		if (!toLeft) // No left one at the moment, so check for any link now...
			m_pJigsaw->applyToAll([&](DisplayableObject* ob) { if (ob != nullptr) this->checkJoinLeft((JigsawPiece*)ob); });
		if (!toTop) // No left one at the moment, so check for any link now...
			m_pJigsaw->applyToAll([&](DisplayableObject* ob) { if (ob != nullptr) this->checkJoinTop((JigsawPiece*)ob); });
		if (!toRight) // No left one at the moment, so check for any link now...
			m_pJigsaw->applyToAll([&](DisplayableObject* ob) { if (ob != nullptr) this->checkJoinRight((JigsawPiece*)ob); });
		if (!toBottom) // No left one at the moment, so check for any link now...
			m_pJigsaw->applyToAll([&](DisplayableObject* ob) { if (ob != nullptr) this->checkJoinBottom((JigsawPiece*)ob); });
	}

	/* Override if necessary - Mouse event - mouse button released. Must call notifyObjectsAboutMouse to tell base engine to notify objects, or this will not be called */
	virtual void virtMouseUp(int iButton, int iX, int iY)
	{
		//std::cout << "virtMouseUp " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
		if (m_bIsBeingDragged)
		{
			virtMouseMoved( iX, iY ); // Do the drag to the position - redrawing it (calling move prevents needing to copy the code from above)
			//std::cout << "Mouse up " << std::to_string(iX) << " " << std::to_string(iY) << std::endl;
			// Now round off the position
			int iXMod = (m_iCurrentScreenX + 10013) % 25 - 13;
			int iYMod = (m_iCurrentScreenY + 10013) % 25 - 13;
			virtMouseMoved( iX - iXMod, iY - iYMod ); // Do the drag to this new aligned position too now
			checkFinishDrag(iX, iY);
			// Check for linking up with any new ones...
			checkForLinksWithOthers();
		}
		//redrawDisplay();
	}

	// Overridden, to call redrawRectangle rather than redrawScreen!
	virtual void doDragRedraw() override
	{
		redrawRectangle(); // It moved so do a redraw!
	}

	// Set the piece filter for the piece - to do the nobbles
	void setFilter(PieceFilter* pFilter) { m_pFilter = pFilter; }

	// Join this to the one on the left
	void joinLeft(JigsawPiece* other) { this->toLeft = other; other->toRight = this; }
	// Join this to the one on the right
	void joinRight(JigsawPiece* other) { this->toRight = other; other->toLeft = this; }
	// Join this to the one above
	void joinTop(JigsawPiece* other) { this->toTop = other; other->toBottom = this; }
	// Join this to the one on the right
	void joinBottom(JigsawPiece* other) { this->toBottom = other; other->toTop = this; }

	// Check whether we can join this other piece to the bottom of ours
	void checkJoinBottom(JigsawPiece* other)
	{ 
		if (other == nullptr) return;
		if (toBottom) return;
		if ((m_iCurrentScreenX == other->m_iCurrentScreenX) // Same X
			&& (m_iXInImage == other->m_iXInImage) // Same X
			&& (m_iCurrentScreenY == other->m_iCurrentScreenY + 50) // The other one should be below this one now
			&& (m_iYInImage == other->m_iYInImage + 50) ) // The other one should be below this one now
		{
			joinBottom(other);
			std::cout << "checkJoinBottom success" << std::endl;
		}
	}

	// Check whether we can join this other piece to the top of ours
	void checkJoinTop(JigsawPiece* other)
	{
		if (other == nullptr) return;
		if (toTop) return;
		if ((m_iCurrentScreenX == other->m_iCurrentScreenX) // Same X
			&& (m_iXInImage == other->m_iXInImage) // Same X
			&& (m_iCurrentScreenY == other->m_iCurrentScreenY - 50) // The other one should be below this one now
			&& (m_iYInImage == other->m_iYInImage - 50)) // The other one should be below this one now
		{
			joinTop(other);
			std::cout << "checkJoinTop success" << std::endl;
		}
	}

	// Check whether we can join this other piece to the right of ours
	void checkJoinRight(JigsawPiece* other)
	{
		if (other == nullptr) return;
		if (toRight) return;
		if ((m_iCurrentScreenY == other->m_iCurrentScreenY) // Same Y
			&& (m_iYInImage == other->m_iYInImage) // Same Y
			&& (m_iCurrentScreenX == other->m_iCurrentScreenX - 50) // The other one should be to the right of this one now
			&& (m_iXInImage == other->m_iXInImage - 50)) // The other one should be to the right in the image
		{
			joinRight(other);
			std::cout << "checkJoinRight success" << std::endl;
		}
	}

	// Check whether we can join this other piece to the left of ours
	void checkJoinLeft(JigsawPiece* other)
	{
		if (other == nullptr) return;
		if (toLeft) return;
		//if ((m_iYInImage == other->m_iYInImage) // Same Y
		//	&& (m_iXInImage == other->m_iXInImage + 50)) // The other one should be to the left in the image
		//{
		//	std::cout << "x " << m_iCurrentScreenX << " vs " << other->m_iCurrentScreenX << std::endl;
		//	std::cout << "y " << m_iCurrentScreenY << " vs " << other->m_iCurrentScreenY << std::endl;
		//}
		if ((m_iCurrentScreenY == other->m_iCurrentScreenY) // Same Y
			&& (m_iYInImage == other->m_iYInImage) // Same Y
			&& (m_iCurrentScreenX == other->m_iCurrentScreenX + 50) // The other one should be to the left of this one now
			&& (m_iXInImage == other->m_iXInImage + 50)) // The other one should be to the left in the image
		{
			joinLeft(other);
			std::cout << "checkJoinLeft success" << std::endl;
		}
	}

	int getXInImage() {	return m_iXInImage; }
	int getYInImage() { return m_iYInImage; }

protected:
	int hack3[128];

	int m_iXInImage;
	int m_iYInImage;
	int m_iWidthInImage;
	int m_iHeightInImage;
	int m_iRotation;
	PieceFilter* m_pFilter;

	int hack1[128];

	// Pointers to related pieces
	JigsawPiece* toLeft = nullptr;
	JigsawPiece* toRight = nullptr;
	JigsawPiece* toTop = nullptr;
	JigsawPiece* toBottom = nullptr;

	int hack2[128];

	// Temp variable used to work out which pieces have already moved for the recursion
	bool m_bMoved = false;
};

