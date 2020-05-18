//
// Created by Chris Purdy on 25/03/2020.
//

#ifndef KSP_2D_CW4_ANIMATEDSPRITE_H
#define KSP_2D_CW4_ANIMATEDSPRITE_H

#include "../../src/SimpleImage.h"
#include "../../src/ImageManager.h"

class AnimatedSprite {
public:
    AnimatedSprite(const char* imgUrl, int _frames, int ticksPerFrame) : frames(_frames), countPerFrame(ticksPerFrame), animationCounter(0) {
        spriteSheet = ImageManager::get()->getImagebyURL(imgUrl, true, false);
        spriteSheet.setTransparencyColour(0x000000);
    }

    void draw(BaseEngine* pEngine, DrawingSurface* surface, int x, int y) {
        spriteSheet.renderImageWithMask(surface,
                            ((animationCounter/countPerFrame) % frames) * (spriteSheet.getWidth()/frames), 0,
                            x, y,
                            spriteSheet.getWidth()/frames, spriteSheet.getHeight());
        ++animationCounter;
    }

    int animationCounter;
    int countPerFrame;
    int frames;
    SimpleImage spriteSheet;
};

#endif //KSP_2D_CW4_ANIMATEDSPRITE_H
