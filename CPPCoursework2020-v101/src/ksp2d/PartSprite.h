//
// Created by Chris Purdy on 09/03/2020.
//

#ifndef KSP_2D_CW4_PARTSPRITE_H
#define KSP_2D_CW4_PARTSPRITE_H

#include "../../sdl2_framework/header.h"
#include "../../sdl2_framework/DragableObject.h"
#include "../Spacecraft/AnimatedSprite.h"

class PartSprite : public DragableObject {

    static uint32_t idCount;

public:

    PartSprite(std::string partName, BaseEngine *pEngine, int initX, int initY);
    ~PartSprite() override;

    std::shared_ptr<SpriteData> data = nullptr;
    bool locked = false;

    std::string getSpriteName();

    void virtDraw() override;

    void virtMouseDown(int iButton, int iX, int iY) override;

    void virtMouseMoved(int iX, int iY) override;

    void virtMouseUp(int iButton, int iX, int iY) override;

    uint8_t id;

    SimpleImage image;

    bool isBeingDragged();

    void drawOnSurface(DrawingSurface *surface);

    void movePosition(int i, int i1);
};


#endif //KSP_2D_CW4_PARTSPRITE_H
