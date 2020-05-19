//
// Created by Chris Purdy on 22/03/2020.
//

#ifndef KSP_2D_CW4_CELESTIALBODY_H
#define KSP_2D_CW4_CELESTIALBODY_H


#include <string>
#include "header.h"
#include "BaseEngine.h"
#include "DisplayableObject.h"
#include "Vec2D.h"
#include "PhysObject.h"

class CelestialBody : public DisplayableObject, public PhysObject {
public:
    CelestialBody(BaseEngine *pEngine, long double x, long double y, const char *_name, const char* imgPath, const char* groundImgPath, long double _radius, bool _hasAtmosphere, Vec2D* origin);

    ~CelestialBody() override;

    enum DrawState {closeBy, farOff, none};

    void virtDraw() override;
    static bool isOnScreen(Vec2D& screenVec, long double screenRadius);

    void virtDoUpdate(int iCurrentTime) override;

    std::string name;

    DrawState drawState;
    bool isCloseByBody;
    double gravityAtSurface;
    long double circumference;
    long double screenDistShipToSurface; // the distance from the ship to the surface of the body in pixels on the screen

    SimpleImage image;
    SimpleImage groundImage;

    void setCloseByBody(bool b);
    Vec2D getPositionOnSurface(double radians, long double heightAbove);

    bool hasAtmosphere;
};

#endif //KSP_2D_CW4_CELESTIALBODY_H
