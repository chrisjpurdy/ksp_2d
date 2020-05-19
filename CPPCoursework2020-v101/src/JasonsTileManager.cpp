#include "header.h"

// This is Jason's version of the MyTileManager class from Demo A.
// It may be a useful comparison for your own class?

#include "JasonsTileManager.h"



JasonsTileManager::JasonsTileManager()
	: TileManager( 20, 20, 15, 15 )
{
}


JasonsTileManager::~JasonsTileManager()
{
}

void JasonsTileManager::virtDrawTileAt(
	BaseEngine* pEngine, // We don't need this but maybe a student will so it is here to use if needed
	DrawingSurface* pSurface,
	int iMapX, int iMapY,
	int iStartPositionScreenX, int iStartPositionScreenY) const
{
	int iMapValue = getMapValue(iMapX, iMapY);
	unsigned int iColour = (unsigned int)((iMapValue & 0xf00) << 12) // red
		+ (unsigned int)((iMapValue & 0xf0) << 8) // green
		+ (unsigned int)((iMapValue & 0xf) << 4); // blue
	pSurface->drawOval(
		iStartPositionScreenX,	// Left
		iStartPositionScreenY,	// Top
		iStartPositionScreenX + getTileWidth() - 1,	// Right
		iStartPositionScreenY + getTileHeight() - 1,	// Bottom
		iColour);
}

