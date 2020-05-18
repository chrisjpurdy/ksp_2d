//
// Created by Chris Purdy on 21/04/2020.
//

#include "PlanetObject.h"

#include <utility>
#include "../KSP2D.h"

PlanetObject::PlanetObject(BaseEngine* pEngine, CelestialBody* _planet, double _positionOnSurface, std::string spriteName, long double widthMult, long double heightMult, long double _groundSlop)
            : DisplayableObject(pEngine), planet(_planet), positionOnSurface(_positionOnSurface), data(ImageManager::get()->getSpriteData(std::move(spriteName))) {
    image = ImageManager::get()->getSpriteFromImg(data);
    image.setTransparencyColour(0x000000);
    width = widthMult * data->width;
    height = heightMult * data->height;
    groundSlop = _groundSlop;
}

void PlanetObject::virtDraw() {
    if (!isVisible()) return;
    // check if planet is closeUpBody (is close by the player-controlled ship), or if the screen is not centred on the ship
    if (!planet->isCloseByBody || reinterpret_cast<KSP2D*>(m_pEngine)->gravObjViewCenter != -1) return;
    // check if the planet is using close-by drawing (drawing surface)
    if (planet->drawState != CelestialBody::DrawState::closeBy) return;
    // check if size is larger than 3 pixels
    if (width*width + height*height <= planet->distMult * 9) return;

    long double screenHeight = height/planet->distMult;

    // check if the planets pos + parentRelativePos is on screen, if so draw the object
    if (planet->screenDistShipToSurface - screenHeight > screenCenter.y) return;

    long double screenWidth = width/planet->distMult;
    long double metresFromShip = (positionOnSurface - reinterpret_cast<KSP2D*>(m_pEngine)->closeByRotation) * reinterpret_cast<Circle*>(planet->shape)->radius;

    image.renderImageBlit(m_pEngine, m_pEngine->getForegroundSurface(), screenCenter.x + metresFromShip/planet->distMult,
                          screenCenter.y + planet->screenDistShipToSurface + (groundSlop/planet->distMult - screenHeight), screenWidth, screenHeight,
            0, 0, data->width, data->height);
}