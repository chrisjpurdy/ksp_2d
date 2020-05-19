//
// Created by Chris Purdy on 09/03/2020.
//

#ifndef KSP_2D_CW4_SPACECRAFT_H
#define KSP_2D_CW4_SPACECRAFT_H

#include "RocketPart.h"
#include "AnimatedSprite.h"
#include "PhysObject.h"
#include "KSPFilterPoints.h"
#include "DrawingFilters.h"
#include "CelestialBody.h"

class KSP2D;

class Spacecraft : public DisplayableObject, public PhysObject {
public:

    Spacecraft(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, int width, int height, Vec2D *origin,
            std::vector<RocketPart*>& _parts, Vec2D topleft, Vec2D bottomright);
    ~Spacecraft() override;

    KSP2D* getKSPEngine();

    double zoomAmount;
    bool isChanged;

    int shipWidth, shipHeight;
    std::string shipName;

    double thrustPercent;
    bool thrustSwitchedOff;

    void virtDoUpdate(int iCurrentTime) override;

    void virtDraw() override;

    static bool isOnScreen(Vec2D &screenPos, long double screenWidth, long double screenHeight);

    void applyGadgets();

    DrawingSurface surface;
    DrawingSurface partsSurface;
    FilterPointsRotate spacecraftRotateFilter;
    FilterPointsFillIn spacecraftFillFilter;
    FilterPointsZoom spacecraftZoomFilter;

    bool checkProximityChange(CelestialBody *planet);
    bool checkSurfaceCollision(CelestialBody *planet);
    void checkAtmosphericForces(CelestialBody *planet);

    std::vector<RocketPart*> parts;
    AeroBehaviour aeroProfile[4]{};
    AnimatedSprite thrustSprite;

    Vec2D relativeVelocity;
    long double shipToSurfaceDist;

    SpacecraftStateMod stateMod{};

    void redrawParts();

    void calcAeroProfile();

    void updateThrusters();

    void applySpacecraftMod();

    void rotate(int direction);
};


#endif //KSP_2D_CW4_SPACECRAFT_H
