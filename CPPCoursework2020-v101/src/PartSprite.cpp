//
// Created by Chris Purdy on 09/03/2020.
//

#include "PartSprite.h"
#include "AnimatedSprite.h"
#include <utility>

uint32_t PartSprite::idCount = 0;

PartSprite::PartSprite(std::string partName, BaseEngine *pEngine, int initX, int initY) : DragableObject (pEngine),
                                    data(ImageManager::get()->getSpriteData(std::move(partName))), id(idCount++) {
    m_iCurrentScreenX = initX;
    m_iCurrentScreenY = initY;
    image = ImageManager::get()->getSpriteFromImg(data);
    setSize(data->width, data->height);
    image.setTransparencyColour(0x000000);
}

void PartSprite::virtDraw() {
    if (isVisible()) {
        drawOnSurface(getEngine()->getForegroundSurface());
    }
}

void PartSprite::drawOnSurface(DrawingSurface *surface) {
    image.renderImageWithMask(surface, 0,0, m_iCurrentScreenX, m_iCurrentScreenY, data->width, data->height, 0x000000);
    /*image.renderImageBlit(getEngine(), surface,
                          m_iCurrentScreenX, m_iCurrentScreenY,
                          data->width, data->height, 0, 0,
                          data->width, data->height);*/
}

void PartSprite::virtMouseDown(int iButton, int iX, int iY) {
    DragableObject::virtMouseDown(iButton, iX, iY);
}

void PartSprite::virtMouseMoved(int iX, int iY) {
    if(locked) {
        // tell spacecraftparts & partcatalogue about it?
        // if (m_bIsBeingDragged) std::cout << "IM LOCKED" << std::endl;
    } else {
        // check proximity to the spacecraft object being built up
        if (m_bIsBeingDragged) DragableObject::virtMouseMoved(iX, iY);
    }
}

void PartSprite::virtMouseUp(int iButton, int iX, int iY) {
    DragableObject::virtMouseUp(iButton, iX, iY);
}

std::string PartSprite::getSpriteName() {
    return data->name;
}

bool PartSprite::isBeingDragged() {
    return m_bIsBeingDragged;
}

PartSprite::~PartSprite() {
    getEngine()->removeDisplayableObject(this);
}

void PartSprite::movePosition(int x, int y) {
    setPosition(m_iCurrentScreenX + x, m_iCurrentScreenY + y);
}
