//
// Created by Chris Purdy on 09/03/2020.
//

#ifndef KSP_2D_CW4_SPACECRAFT_H
#define KSP_2D_CW4_SPACECRAFT_H

#include "../RocketPart/RocketPart.h"
#include "AnimatedSprite.h"
#include "../PhysUtils/PhysObject.h"
#include "../KSPFilterPoints.h"
#include "../../sdl2_framework/DrawingFilters.h"
#include "../Orbital/CelestialBody.h"

class KSP2D;

class Spacecraft : public DisplayableObject, public PhysObject {
public:

    Spacecraft(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, long double mass, Vec2D *origin, std::vector<RocketPart*>& _parts);
    ~Spacecraft() override;

    KSP2D* getKSPEngine();

    double zoomAmount;
    bool isChanged;

    double thrustPercent;
    double thrustStrength;
    bool thrustSwitchedOff;

    void virtDoUpdate(int iCurrentTime) override;

    void virtDraw() override;

    static bool isOnScreen(Vec2D &screenPos, long double screenWidth, long double screenHeight);

    void rotate(double angle, long double timeMod);

    // TODO use weak pointers to store Gadget shared pointers contained in the rocket parts - means when the rocket
    //  parts are deleted the gadgets weak pointers expire
    void accumulateGadgets();
    SpacecraftStateMod applyGagets();

    DrawingSurface surface;
    DrawingSurface partsSurface;
    FilterPointsRotate spacecraftRotateFilter;
    FilterPointsWithinSurfaceBounds spacecraftBoundsFilter;
    FilterPointsZoom spacecraftZoomFilter;

    bool checkProximityChange(CelestialBody *planet);
    void checkSurfaceCollision(CelestialBody *planet);
    void checkAtmosphericForces(CelestialBody *planet);

    std::vector<RocketPart*> parts;
    AeroBehaviour aeroProfile[4]{};
    AnimatedSprite thrustSprite;

    void redrawParts();

    void calcAeroProfile();

    void applyThrust();
};


#endif //KSP_2D_CW4_SPACECRAFT_H
