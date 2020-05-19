//
// Created by Chris Purdy on 22/03/2020.
//

#ifndef KSP_2D_CW4_SUN_H
#define KSP_2D_CW4_SUN_H

#include "CelestialBody.h"

class Sun : public CelestialBody {
public:

    Sun(BaseEngine *pEngine, const char* name, const char* imgPath, const char* groundImgPath, long double radius, long double mass, long double x, long double y,
            Vec2D *origin) : CelestialBody(pEngine, x, y, name, imgPath, groundImgPath, radius, false, origin) {
        body = new PhysBody(dynamic_cast<PhysObject*>(this), mass, Vec2D(x, y), 0);
        body->setStatic();
    }

};

#endif //KSP_2D_CW4_SUN_H
