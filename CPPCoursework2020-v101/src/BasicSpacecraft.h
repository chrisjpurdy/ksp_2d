//
// Created by Chris Purdy on 19/03/2020.
//

#ifndef KSP_2D_CW4_BASICSPACECRAFT_H
#define KSP_2D_CW4_BASICSPACECRAFT_H

#include "../../sdl2_framework/SimpleImage.h"
#include "../../sdl2_framework/ImageManager.h"
#include "../../sdl2_framework/BaseEngine.h"
#include "../KSP2D.h"
#include "AnimatedSprite.h"

/**
 * DEPRECATED - now using Spacecraft since the spacecraft builder works
 */
class BasicSpacecraft : public DisplayableObject, public PhysObject {
public:
    BasicSpacecraft(KSP2D *pEngine, std::string spriteName, const Vec2D &initalPos, const Vec2D &initialVel,
                    long double mass, Vec2D *origin);

    void virtDraw() override;

    KSP2D* getKSPEngine();

    double zoomAmount;
    bool isChanged;

    double thrustPercent;
    double thrustStrength;
    bool thrustSwitchedOff;

    SimpleImage rocketSprite;
    AnimatedSprite thrustSprite;
    std::shared_ptr<SpriteData> data = nullptr;

    void virtDoUpdate(int iCurrentTime) override;

    static bool isOnScreen(Vec2D &screenPos, long double screenWidth, long double screenHeight);

    void rotate(double angle);

    void applyThrust();

    DrawingSurface surface;
    FilterPointsRotate spacecraftRotateFilter;
    FilterPointsWithinSurfaceBounds spacecraftBoundsFilter;
    FilterPointsZoom spacecraftZoomFilter;

    bool checkProximityChange(CelestialBody *planet);
    void checkSurfaceCollision(CelestialBody *planet);
    void checkAtmosphericForces(CelestialBody *planet);
};


#endif //KSP_2D_CW4_BASICSPACECRAFT_H
