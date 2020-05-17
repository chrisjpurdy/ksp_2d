//
// Created by Chris Purdy on 08/03/2020.
//

#ifndef KSP_2D_CW4_KSP2D_H
#define KSP_2D_CW4_KSP2D_H

#include "../sdl2_framework/header.h"
#include "../sdl2_framework/BaseEngine.h"
#include "../sdl2_framework/SimpleTrig.h"
#include "RocketPart/PartSprite.h"
#include "Orbital/Planet.h"
#include "Orbital/Sun.h"
#include "Spacecraft/Spacecraft.h"
#include "KSPFilterPoints.h"
#include "../sdl2_framework/DrawingFilters.h"
#include "PlanetObjects/PlanetObject.h"

class KSP2D : public BaseEngine {
public:

    KSP2D();

    ~KSP2D() override = default;

    void virtSetupBackgroundBuffer() override;
    void virtCleanUp() override;
    int virtInitialise() override;
    int virtInitialiseObjects() override;
    void virtMainLoopPreUpdate() override;
    void virtMainLoopDoBeforeUpdate() override;
    void virtMainLoopPostUpdate() override;
    void virtPostDraw() override;
    void virtMouseDown(int iButton, int x, int y) override;
//    void virtKeyDown(int iKeyCode) override;
//    void virtKeyUp(int iKeyCode) override;

    void speedUpGravObjs(long double timeMod);

    void setPlanetScale(long double distMod, Vec2D* origin);

    void checkKeyInputs();

    enum State { stateInit, stateBuilder, stateTakeoff, stateFlight, stateReentry, stateLowOrbit, stateOrbit, stateLanded }; // reentry sequence is probably same as flight

    // TODO allow for relative movements of camera relative to an orbit view center (a planet or a ship - anything containing a GravSphere object)
    Vec2D relativeOrbitViewCentre;
    Vec2D freeMovingOrbitViewCentre;
    Vec2D* orbitViewCentre;
    long double distModifier, timeModifier;
    bool changeOriginLock;
    int gravObjViewCenter;

    bool reposSpacecraft;

    State state;
    std::vector<CelestialBody*> planets;
    std::vector<PlanetObject*> planetDecorations;
    std::vector<Spacecraft*> spacecraft;

    Spacecraft* playerSpacecraft = nullptr;
    CelestialBody* closeByBody = nullptr;
    bool bgCleared;
    double closeByRotation = 0;

    long double maxDistanceMod = 10000000000;
    long double minDistanceMod = 1;
    long double maxTimeMod = 1000000;

    // New shape drawing functions
    void drawForegroundEllipse(long int centerX, long int centerY, long int radiusX, long int radiusY, unsigned int uiColour);
    void plot4EllipsePoints(long int centerX, long int centerY, long int x, long int y, unsigned int uiColour);
    void drawForegroundArrow(int x, int y, Vec2D vec, int length, unsigned int uiColour);

    void buildPlayerSpacecraft();
    void addNewSpacecraft();
    SimpleImage factoryBg;

    bool isKeyListener;

    void changeSkyColour(double x);
};

#endif //KSP_2D_CW4_KSP2D_H
