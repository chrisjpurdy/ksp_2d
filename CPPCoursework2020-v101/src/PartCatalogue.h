//
// Created by Chris Purdy on 20/04/2020.
//

#ifndef KSP_2D_CW4_PARTCATALOGUE_H
#define KSP_2D_CW4_PARTCATALOGUE_H

#include "GUIManager.h"
#include "../RocketPart/RocketPart.h"
#include "../RocketPart/PartSprite.h"
#include "../RocketPart/SpacecraftParts.h"
#include "../KSP.h"
#include "../../sdl2_framework/TileManager.h"

class PartCatalogue : public TileManager, public DisplayableObject {
public:
    PartCatalogue(BaseEngine* engine, int x, int y, int _numItems);

    ~PartCatalogue() override;

    void virtDrawTileAt(BaseEngine *pEngine, DrawingSurface *pSurface, int iMapX, int iMapY,
            int iStartPositionScreenX, int iStartPositionScreenY) const override;

    static bool isControllablePart(const std::string& partName) {
         auto partData = KSPMetaManager::get()->getPartData(partName);
        for (auto gadget : *partData->gadgets) {
            if (gadget->isControllable()) {
                return true;
            }
        }
        return false;
    }

    void moveCatalogueView(int offset) {
        partCatalogueOffset += offset;
        if (partCatalogueOffset < 0) partCatalogueOffset += numItems;
        for (int i=0; i<10; ++i) setMapValue(i+1,0, (i+partCatalogueOffset)%numItems);
    }

    void virtDoUpdate(int iCurrentTime) override;

    // ---- Mouse click functions ----

    void virtMouseDown(int iButton, int x, int y) override;
    void virtMouseUp(int iButton, int x, int y) override;
    void virtMouseMoved(int iX, int iY) override;

    void offsetShip(double xOff, double yOff);
    void resetShipOffset();

    /**
     * Displays information about a particular part when hovering over
     *
     * @param pEngine
     * @param pSurface
     */
    void drawPartInfo(BaseEngine *pEngine, DrawingSurface *pSurface);

    Spacecraft* makeShip(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, Vec2D *origin);

    void draw(int iCurrentTime);

    int tileSize;
    int tileBorderOffset = 20;
    int numItems;

    int partCatalogueOffset;

    RocketPart* partBeingDragged;
    bool deletePart;
    SpacecraftParts* spacecraftParts;

    double spacecraftXOff, spacecraftYOff;
    std::vector<RocketPart*> sideMostParts;

    SimpleImage* rightArrow;
    SimpleImage* leftArrow;
    SimpleImage* binSprite;
    int binPosX, binPosY;

    SimpleImage* clipPointer;
    int clipX, clipY;
    bool showPointer;

    int mouseOverX, mouseOverY;
    std::shared_ptr<PartData> hoverPartInfo;
    SimpleImage* hoverBackground;

    SimpleImage* catalogueSpritesheet; // image storing the background of the catalogue
    std::vector<SimpleImage*> catalogueShowcaseSprites; // images of each part in the catalogue
    std::vector<std::string> catalogue; // stores the sprite name of each part to look up using the sprite sheet functionality in ImageManager
};

#endif //KSP_2D_CW4_PARTCATALOGUE_H
