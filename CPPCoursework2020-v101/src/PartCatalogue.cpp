//
// Created by Chris Purdy on 18/05/2020.
//

#include "PartCatalogue.h"

PartCatalogue::PartCatalogue(BaseEngine* engine, int x, int y, int _numItems) : TileManager(80,80),
                DisplayableObject(engine, 0, 0),
                numItems(_numItems), spacecraftParts(nullptr), partBeingDragged(nullptr), deletePart(false),
                showPointer(false), partCatalogueOffset(0), hoverPartInfo(nullptr) {
    catalogueSpritesheet = new SimpleImage(ImageManager::get()->loadImage("spacecraftBuilderUIPanels.png"));
    setMapSize(12,1);
    setTopLeftPositionOnScreen(x, y);
    tileSize = catalogueSpritesheet->getWidth()/2;

    for (int i=0; i<4; i++) sideMostParts.push_back(nullptr);

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

    spacecraftXOff = 0.1;
    spacecraftYOff = 0.1;
}

PartCatalogue::~PartCatalogue() {
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
}

void PartCatalogue::draw(int iCurrentTime) {
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

Spacecraft* PartCatalogue::makeShip(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, Vec2D *origin) {
    if (!spacecraftParts) return nullptr;
    // first centre spacecraft parts and the offset x by the root parts distance from the x centre
    resetShipOffset();

    double xCent = spacecraftParts->parts[0]->sprite->getXCentre();
    double yCent = ((sideMostParts[RocketPart::Side::top]->sprite->getYCentre() - sideMostParts[RocketPart::Side::top]->sprite->getDrawHeight()/2.0) +
                    (sideMostParts[RocketPart::Side::bottom]->sprite->getYCentre() + sideMostParts[RocketPart::Side::bottom]->sprite->getDrawHeight()/2.0))/2.0;

    Vec2D tl(xCent - (sideMostParts[RocketPart::Side::left]->sprite->getXCentre() - sideMostParts[RocketPart::Side::left]->sprite->getDrawWidth()/2.0),
             yCent - (sideMostParts[RocketPart::Side::top]->sprite->getYCentre() - sideMostParts[RocketPart::Side::top]->sprite->getDrawHeight()/2.0));
    Vec2D br(xCent - (sideMostParts[RocketPart::Side::right]->sprite->getXCentre() + sideMostParts[RocketPart::Side::right]->sprite->getDrawWidth()/2.0),
             yCent - (sideMostParts[RocketPart::Side::bottom]->sprite->getYCentre() + sideMostParts[RocketPart::Side::bottom]->sprite->getDrawHeight()/2.0));

    int xOff = screenCenter.x - xCent + 0.01;
    int yOff = screenCenter.y - yCent + 0.01;
    offsetShip(xOff, yOff);

    int width = (sideMostParts[RocketPart::Side::right]->sprite->getXCentre() + sideMostParts[RocketPart::Side::right]->sprite->getDrawWidth()/2)
                - (sideMostParts[RocketPart::Side::left]->sprite->getXCentre() - sideMostParts[RocketPart::Side::left]->sprite->getDrawWidth()/2);
    int height = (sideMostParts[RocketPart::Side::bottom]->sprite->getYCentre() + sideMostParts[RocketPart::Side::bottom]->sprite->getDrawHeight()/2)
                 - (sideMostParts[RocketPart::Side::top]->sprite->getYCentre() - sideMostParts[RocketPart::Side::top]->sprite->getDrawHeight()/2);

    return spacecraftParts->generateSpacecraft(pEngine, initalPos, initialVel, width, height, origin, tl, br);
}

/**
     * Displays information about a particular part when hovering over
     *
     * @param pEngine
     * @param pSurface
     */
void PartCatalogue::drawPartInfo(BaseEngine *pEngine, DrawingSurface *pSurface) {
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
            pSurface->drawFastString(topX + 22, topY + 45, (std::string("Mass (t): ") + std::to_string(hoverPartInfo->mass)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            pSurface->drawFastString(topX + 22, topY + 70, (std::string("Max temperature: ") + std::to_string(hoverPartInfo->maxTemperature)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            pSurface->drawFastString(topX + 22, topY + 95, "Resources: ", 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            pSurface->drawFastString(topX + 32, topY + 120, (std::string("- Fuel: ") + std::to_string(hoverPartInfo->resources.maxFuel)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            pSurface->drawFastString(topX + 32, topY + 145, (std::string("- Monopropellant: ") + std::to_string(hoverPartInfo->resources.maxMonoprop)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            pSurface->drawFastString(topX + 32, topY + 170, (std::string("- Electricity: ") + std::to_string(hoverPartInfo->resources.maxElectricity)).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            pSurface->drawFastString(topX + 22, topY + 195, "Gadgets:", 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            for (int i=0; i<hoverPartInfo->gadgets->size(); i++) {
                pSurface->drawFastString(topX + 32, topY + 220 + i*25, (std::string("- ") + hoverPartInfo->gadgets->at(i)->gadgetName).c_str(), 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            }
            if (hoverPartInfo->gadgets->empty()) {
                pSurface->drawFastString(topX + 32, topY + 220, "- none", 0xffffff, pEngine->getFont("kenpixel.ttf", 16));
            }
        }
    }
}

void PartCatalogue::virtMouseMoved(int iX, int iY) {
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

void PartCatalogue::virtDrawTileAt(BaseEngine *pEngine, DrawingSurface *pSurface, int iMapX, int iMapY,
                                   int iStartPositionScreenX, int iStartPositionScreenY) const {

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

void PartCatalogue::virtDoUpdate(int iCurrentTime) {

    if (!isVisible()) return;

    // check if any parts in spacecraftparts are being dragged (if spacecraftparts exists)
    if (!partBeingDragged && spacecraftParts) {
        partBeingDragged = spacecraftParts->getDraggedPart(sideMostParts);
        if (partBeingDragged) {
            // if the first part is being lifted then delete the spacecraft parts
            if (spacecraftParts->getNumParts() == 0) {
                partBeingDragged->isRootPart = false;
                delete spacecraftParts;
                spacecraftParts = nullptr;
                for (int i=0; i<4; i++) sideMostParts[i] = nullptr;
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

void PartCatalogue::virtMouseUp(int iButton, int x, int y) {

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
            spacecraftParts = new SpacecraftParts(screenCenter.x, screenCenter.y, partBeingDragged);
            for (int i=0; i<4; i++) {
                sideMostParts[i] = partBeingDragged;
            }
            partBeingDragged->isRootPart = true;
            partBeingDragged->sprite->locked = true;
            partBeingDragged = nullptr;
        } else {
            auto snapPoint = spacecraftParts->getClosestSnapTo(x, y, partBeingDragged);
            if (snapPoint.first) {
                if (spacecraftParts->connect(partBeingDragged, snapPoint.first, snapPoint.second)) {
                    // part was added to the spacecraft parts

                    if (spacecraftParts->isMoreSideMost(partBeingDragged, snapPoint.second, sideMostParts)) {
                        sideMostParts[snapPoint.second] = partBeingDragged;
                        spacecraftParts->reposParts(partBeingDragged->sprite->getDrawWidth()/2,
                                                    partBeingDragged->sprite->getDrawHeight()/2, snapPoint.second,
                                                    sideMostParts);
                    }

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

void PartCatalogue::virtMouseDown(int iButton, int x, int y) {

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

void PartCatalogue::offsetShip(double xOff, double yOff) {

    if (!spacecraftParts) return;

    for (auto part : spacecraftParts->parts) {
        part->sprite->movePosition(-spacecraftXOff, -spacecraftYOff);
    }

    spacecraftXOff += xOff;
    spacecraftYOff += yOff;

    if (abs(spacecraftXOff) > screenCenter.x - 100) {
        spacecraftXOff *= (screenCenter.x - 100)/abs(spacecraftXOff);
    }
    if (abs(spacecraftYOff) > screenCenter.y - 100) {
        spacecraftYOff *= (screenCenter.y - 100)/abs(spacecraftYOff);
    }

    for (auto part : spacecraftParts->parts) {
        part->sprite->movePosition(spacecraftXOff, spacecraftYOff);
    }
}

void PartCatalogue::resetShipOffset() {
    if (!spacecraftParts) return;

    for (auto part : spacecraftParts->parts) {
        part->sprite->movePosition(-spacecraftXOff, -spacecraftYOff);
    }

    spacecraftXOff = 0.1;
    spacecraftYOff = 0.1;
}