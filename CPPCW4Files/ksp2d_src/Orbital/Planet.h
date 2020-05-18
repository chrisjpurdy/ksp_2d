//
// Created by Chris Purdy on 16/03/2020.
//

#ifndef KSP_2D_CW4_PLANET_H
#define KSP_2D_CW4_PLANET_H

#include "CelestialBody.h"

class Planet : public CelestialBody {
public:

    Planet(BaseEngine* pEngine, const char* name, const char* imgPath, const char* groundImgPath, long double radius, long double mass, long double x, long double y, Vec2D* origin, double eccentricity, long double orbitTime, double initialAngleOffset, CelestialBody* parentBody)
            : CelestialBody(pEngine, x, y, name, imgPath, groundImgPath, radius, origin) {
        body = new RailedPhysBody(dynamic_cast<PhysObject*>(this), mass, Vec2D(x, y), 0, parentBody->body, eccentricity, orbitTime, initialAngleOffset);
        body->setStatic();
    }

};


#endif //KSP_2D_CW4_PLANET_H
