//
// Created by Chris Purdy on 20/04/2020.
//

#ifndef KSP_2D_CW4_PARTCATALOGUE_H
#define KSP_2D_CW4_PARTCATALOGUE_H

#include "../../sdl2_framework/TileManager.h"
#include "GUIManager.h"
#include "../RocketPart/RocketPart.h"
#include "../RocketPart/PartSprite.h"
#include "../RocketPart/SpacecraftParts.h"
#include "../KSP.h"

class PartCatalogue : public TileManager, public DisplayableObject {
public:
    PartCatalogue(BaseEngine* engine, int x, int y, int _numItems) : TileManager(80,80), DisplayableObject(engine, 0, 0),
                        numItems(_numItems), spacecraftParts(nullptr), partBeingDragged(nullptr), deletePart(false),
                        showPointer(false), partCatalogueOffset(0), hoverPartInfo(nullptr) {
        catalogueSpritesheet = new SimpleImage(ImageManager::get()->loadImage("spacecraftBuilderUIPanels.png"));
        setMapSize(12,1);
        setTopLeftPositionOnScreen(x, y);
        tileSize = catalogueSpritesheet->getWidth()/2;

        // TODO setup catalogue names from elsewhere, load in from a file etc?
        for(int i=0; i<5; ++i) catalogue.push_back(std::string("bodypart_") + std::to_string(i));
        for(int i=0; i<3; ++i) catalogue.push_back(std::string("noseCone_") + std::to_string(i));
        for(int i=0; i<5; ++i) catalogue.push_back(std::string("thruster_") + std::to_string(i));
        for(int i=0; i<4; ++i) catalogue.push_back(std::string("sideThruster_") + std::to_string(i));
        for(int i=0; i<1; ++i) catalogue.push_back(std::string("decoupler_") + std::to_string(i));

        setMapValue(0, 0, -1);
        setMapValue(11, 0, -2);
        for (int i=0; i<10; ++i) setMapValue(i+1,0, i);

        // setup sprite showcases from catalogue names
        for(int i=0; i<numItems; ++i) {
            auto data = ImageManager::get()->getSpriteData(catalogue.at(i));
            catalogueShowcaseSprites.push_back(new SimpleImage(ImageManager::get()->getSpriteFromImg(data)));
            catalogueShowcaseSprites.at(i)->setTransparencyColour(0x000000);
        }
        catalogueSpritesheet->setTransparencyColour(0x000000);

        auto data = ImageManager::get()->getSpriteData("bin");
        binSprite = new SimpleImage(ImageManager::get()->getSpriteFromImg(data));
        binSprite->setTransparencyColour(0x000000);
        binPosX = x + m_iMapWidth * 80 + 10;
        binPosY = y + m_iMapHeight * 80 - data->height * 2;

        leftArrow = new SimpleImage(ImageManager::get()->getSpriteFromImg(ImageManager::get()->getSpriteData("greyArrowLeft")));
        leftArrow->setTransparencyColour(0x000000);

        rightArrow = new SimpleImage(ImageManager::get()->getSpriteFromImg(ImageManager::get()->getSpriteData("greyArrowRight")));
        rightArrow->setTransparencyColour(0x000000);

        clipPointer = new SimpleImage(ImageManager::get()->getSpriteFromImg(ImageManager::get()->getSpriteData("crosshair_0")));
        clipPointer->setTransparencyColour(0x000000);

        hoverBackground = new SimpleImage(ImageManager::get()->getSpriteFromImg(ImageManager::get()->getSpriteData("panel_0")));
        hoverBackground->setTransparencyColour(0x000000);
    }

    ~PartCatalogue() override {
        m_pEngine->removeDisplayableObject(this);
        delete binSprite;
        delete leftArrow;
        delete rightArrow;
        delete clipPointer;
        delete catalogueSpritesheet;
        delete hoverBackground;
        for(int i=catalogueShowcaseSprites.size()-1; i>=0; --i) {
            delete catalogueShowcaseSprites.at(i);
        }
        delete spacecraftParts;
    };

    void virtDrawTileAt(BaseEngine *pEngine, DrawingSurface *pSurface, int iMapX, int iMapY,
            int iStartPositionScreenX, int iStartPositionScreenY) const override {

        int spriteIndex = getMapValue(iMapX, iMapY);

        if (spriteIndex >= numItems) return;

        if (spriteIndex == -1) {
            leftArrow->renderImageBlit(pEngine, pSurface, iStartPositionScreenX + 0.5*(m_iTileWidth - leftArrow->getWidth()),
                                        iStartPositionScreenY + 0.5*(m_iTileHeight - leftArrow->getHeight()),
                                        leftArrow->getWidth(), leftArrow->getHeight(),0, 0,
                                        leftArrow->getWidth(), leftArrow->getHeight());
        } else if (spriteIndex == -2) {
            rightArrow->renderImageBlit(pEngine, pSurface, iStartPositionScreenX + 0.5*(m_iTileWidth - rightArrow->getWidth()),
                    iStartPositionScreenY + 0.5*(m_iTileHeight - rightArrow->getHeight()),
                    rightArrow->getWidth(), rightArrow->getHeight(),0, 0,
                    rightArrow->getWidth(), rightArrow->getHeight());
        } else {
            catalogueSpritesheet->renderImageBlit(pEngine, pSurface, iStartPositionScreenX, iStartPositionScreenY,
                    m_iTileWidth, m_iTileHeight,
                    0, 0, tileSize, tileSize);

            // check whether the part is the first part, and if it is suitable
            if (spacecraftParts || isControllablePart(catalogue[spriteIndex])) {
                float widthRatio = (float) (m_iTileWidth - tileBorderOffset) / catalogueShowcaseSprites[spriteIndex]->getWidth();
                float heightRatio = (float) (m_iTileHeight - tileBorderOffset) / catalogueShowcaseSprites[spriteIndex]->getHeight();
                float minRatio = std::min(widthRatio, heightRatio);
                int screenWidth = minRatio * catalogueShowcaseSprites[spriteIndex]->getWidth();
                int screenHeight = minRatio * catalogueShowcaseSprites[spriteIndex]->getHeight();

                catalogueShowcaseSprites[spriteIndex]->renderImageBlit(pEngine, pSurface,
                        iStartPositionScreenX + 0.5 * (m_iTileWidth - screenWidth),
                        iStartPositionScreenY + 0.5 * (m_iTileHeight - screenHeight),
                        screenWidth, screenHeight,0, 0,
                        catalogueShowcaseSprites[spriteIndex]->getWidth(),
                        catalogueShowcaseSprites[spriteIndex]->getHeight());
            }
        }
    }

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

    void virtDoUpdate(int iCurrentTime) override {

        if (!isVisible()) return;

        // check if any parts in spacecraftparts are being dragged (if spacecraftparts exists)
        if (!partBeingDragged && spacecraftParts) {
            partBeingDragged = spacecraftParts->getDraggedPart();
            // if the first part is being lifted then delete the spacecraft parts
            if (partBeingDragged) {
                if (spacecraftParts->getNumParts() == 0) {
                    partBeingDragged->isRootPart = false;
                    delete spacecraftParts;
                    spacecraftParts = nullptr;
                }
                partBeingDragged->sprite->locked = false;
            }
            deletePart = false;
        }

        if(partBeingDragged) {
            if (deletePart) {
                m_pEngine->removeDisplayableObject(partBeingDragged->sprite);
                delete partBeingDragged;
                partBeingDragged = nullptr;
                deletePart = false;
            }
        }
    }

    // ---- Mouse click functions ----

    void virtMouseDown(int iButton, int x, int y) override {

        if (!isVisible()) return;

        int tilex = (x - m_iBaseScreenX) / m_iTileWidth;
        float tiley = (float)(y - m_iBaseScreenY) / m_iTileHeight;

        // need to check for whether it is the first part being clicked, if so put in the middle

        if (partBeingDragged) return;

        if (tiley > 0 && tiley < 1) {
            if (tilex >= 1 && tilex < 11) {
                int spriteIndex = getMapValue(tilex, 0);

                if (spriteIndex >= numItems) return;

                if (!spacecraftParts) {
                    if (!isControllablePart(catalogue[spriteIndex])) {
                        GUIManager::get()->fadeTextAlert("Please choose a valid root part (drone core or command pod)");
                        return;
                    }
                }

                auto* newPart = new PartSprite(catalogue[spriteIndex], m_pEngine,
                                               x - catalogueShowcaseSprites[spriteIndex]->getWidth() / 2,
                                               y - catalogueShowcaseSprites[spriteIndex]->getHeight() / 2);
                m_pEngine->appendObjectToArray(newPart);
                newPart->checkStartDrag(x, y);
                partBeingDragged = new RocketPart(newPart);
                deletePart = false;
            } else if (tilex == 0) {
                moveCatalogueView(-1);
            } else if (tilex == 11) {
                moveCatalogueView(1);
            }
        } else {
            partBeingDragged = nullptr;
            deletePart = false;
        }
    }

    void virtMouseUp(int iButton, int x, int y) override {

        if (!isVisible()) return;

        if (partBeingDragged) {

            // first check if over bin, if so delete the part
            if (x > binPosX && x < binPosX + binSprite->getWidth()*2 && y > binPosY && y < binPosY + binSprite->getHeight()*2) {
                // if so delete sprite
                deletePart = true;
                return;
            }
            // then check if it is the first part placed down, or if there is a point on the spacecraft to snap to
            if (!spacecraftParts) {
                //std::cout << "Making new spacecraftparts" << std::endl;
                spacecraftParts = new SpacecraftParts(650, 100, partBeingDragged);
                partBeingDragged->isRootPart = true;
                partBeingDragged->sprite->locked = true;
                partBeingDragged = nullptr;
            } else {
                auto snapPoint = spacecraftParts->getClosestSnapTo(x, y, partBeingDragged);
                if (snapPoint.first) {
                    if (spacecraftParts->connect(partBeingDragged, snapPoint.first, snapPoint.second)) {
                        // part was added to the spacecraft parts
                        partBeingDragged->sprite->locked = true;
                        partBeingDragged = nullptr;
                    } else {
                        deletePart = true;
                    }
                } else {
                    deletePart = true;
                }
            }
        }
    }

    void virtMouseMoved(int iX, int iY) override {
        mouseOverX = iX;
        mouseOverY = iY;
        showPointer = false;
        if (partBeingDragged && spacecraftParts) {
            auto snapPoint = spacecraftParts->getClosestSnapTo(iX, iY, partBeingDragged);
            if (snapPoint.first) {
                clipX = snapPoint.first->sprite->getXCentre();
                clipY = snapPoint.first->sprite->getYCentre();
                snapPoint.first->modifyCentreToAnchorPoint(clipX, clipY, snapPoint.second);
                showPointer = true;
            }
        }
    }

    /**
     * Displays information about a particular part when hovering over
     *
     * @param pEngine
     * @param pSurface
     */
    void drawPartInfo(BaseEngine *pEngine, DrawingSurface *pSurface) {
        int tilex = (mouseOverX - m_iBaseScreenX) / m_iTileWidth;
        float tiley = (float)(mouseOverY - m_iBaseScreenY) / m_iTileHeight;

        if (tiley > 0 && tiley < 1) {
            if (tilex >= 1 && tilex < 11) {
                int spriteIndex = getMapValue(tilex, 0);

                if (hoverPartInfo) {
                    if (catalogue[spriteIndex] != hoverPartInfo->partName) {
                        hoverPartInfo = KSPMetaManager::get()->getPartData(catalogue[spriteIndex]);
                    }
                } else {
                    hoverPartInfo = KSPMetaManager::get()->getPartData(catalogue[spriteIndex]);
                }

                int topX = mouseOverX - (mouseOverX > screenCenter.x ? hoverBackground->getWidth()*4 : 0);
                int topY = mouseOverY - hoverBackground->getHeight()*4;
                hoverBackground->renderImageBlit(pEngine, pSurface, topX, topY, hoverBackground->getWidth()*4, hoverBackground->getHeight()*4,
                        0, 0, hoverBackground->getWidth(), hoverBackground->getHeight());

                pSurface->drawFastString(topX + 22, topY + 20, hoverPartInfo->partName.c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                pSurface->drawFastString(topX + 22, topY + 45, (std::string("Max temperature: ") + std::to_string(hoverPartInfo->maxTemperature)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                pSurface->drawFastString(topX + 22, topY + 70, "Resources: ", 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                pSurface->drawFastString(topX + 32, topY + 95, (std::string("- Fuel: ") + std::to_string(hoverPartInfo->resources.maxFuel)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                pSurface->drawFastString(topX + 32, topY + 120, (std::string("- Monopropellant: ") + std::to_string(hoverPartInfo->resources.maxMonoprop)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                pSurface->drawFastString(topX + 32, topY + 145, (std::string("- Electricity: ") + std::to_string(hoverPartInfo->resources.maxElectricity)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                pSurface->drawFastString(topX + 22, topY + 170, "Gadgets:", 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                for (int i=0; i<hoverPartInfo->gadgets->size(); i++) {
                    pSurface->drawFastString(topX + 32, topY + 195 + i*25, (std::string("- ") + hoverPartInfo->gadgets->at(i)->gadgetName).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                }
                if (hoverPartInfo->gadgets->empty()) {
                    pSurface->drawFastString(topX + 32, topY + 195, "- none", 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
                }
            }
        }
    }

    Spacecraft* makeShip(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, long double mass,
                                                        Vec2D *origin) {
        if (!spacecraftParts) return nullptr;
        return spacecraftParts->generateSpacecraft(pEngine, initalPos, initialVel, mass, origin);
    }

    void draw(int iCurrentTime) {
        if (!isVisible()) return;

        drawAllTiles(m_pEngine, m_pEngine->getForegroundSurface());
        binSprite->renderImageBlit(m_pEngine, m_pEngine->getForegroundSurface(), binPosX, binPosY, binSprite->getWidth()*2, binSprite->getHeight()*2,
                                   0, 0, binSprite->getWidth(), binSprite->getHeight());

        if (partBeingDragged && spacecraftParts && showPointer) {
            clipPointer->renderImageBlit(m_pEngine, m_pEngine->getForegroundSurface(), clipX - clipPointer->getWidth()/2, clipY - clipPointer->getHeight()/2, clipPointer->getWidth(), clipPointer->getHeight(),
                                         0, 0, clipPointer->getWidth(), clipPointer->getHeight());
        }

        drawPartInfo(m_pEngine, m_pEngine->getForegroundSurface());
    }

    int tileSize;
    int tileBorderOffset = 20;
    int numItems;

    int partCatalogueOffset;

    RocketPart* partBeingDragged;
    bool deletePart;
    SpacecraftParts* spacecraftParts;

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
