//
// Created by Chris Purdy on 25/03/2020.
//

#ifndef KSP_2D_CW4_SPACECRAFTCONTROLS_H
#define KSP_2D_CW4_SPACECRAFTCONTROLS_H

#include "../Spacecraft/BasicSpacecraft.h"
#include "GUISlider.h"

class SpacecraftControls {
public:
    SpacecraftControls(KSP2D* pEngine, BasicSpacecraft* _spacecraft);

    ~SpacecraftControls();

    KSP2D* engine;
    BasicSpacecraft* spacecraft;
    GUISlider<double>* thrustSlider;
    GUISlider<long double> *orbitTimeSlider, *orbitDistSlider;
    SimpleImage spacecraftHUD;

    int prevTime;
    double closeAnim;
    double closeAmount;

    bool objectsAdded;

    void draw(int iCurrentTime);

    void addObjectsToEngine();

    void testLock();

    void update(int iCurrentTime);

    void open();

    void close();
};


#endif //KSP_2D_CW4_SPACECRAFTCONTROLS_H
