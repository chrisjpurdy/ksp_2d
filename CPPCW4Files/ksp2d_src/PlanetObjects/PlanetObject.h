//
// Created by Chris Purdy on 21/04/2020.
//

#ifndef KSP_2D_CW4_PLANETOBJECT_H
#define KSP_2D_CW4_PLANETOBJECT_H


#include <utility>

#include "../../src/DisplayableObject.h"
#include "../Orbital/CelestialBody.h"

/**
 *  A class to abstract the concept of a static (decorative) object on a planets surface
 *  positionOnSurface is expressed in radians and refers to the angle between the vector (0,-1) (straight up) and the vector from
 *  the centre of the planet to the object
 */
class PlanetObject : public DisplayableObject {
public:

    PlanetObject(BaseEngine *pEngine, CelestialBody *_planet, double _positionOnSurface, std::string spriteName,
                 long double widthMult, long double heightMult, long double _groundSlop);

    void virtDraw() override;

    double positionOnSurface;
    long double width, height, groundSlop;
    CelestialBody* planet;
    SimpleImage image;
    std::shared_ptr<SpriteData> data;
};


#endif //KSP_2D_CW4_PLANETOBJECT_H
