//
// Created by Chris Purdy on 18/03/2020.
//

#ifndef KSP_2D_CW4_KSP_H
#define KSP_2D_CW4_KSP_H

#include "PhysUtils/Vec2D.h"
#define M_PI 3.1415927

const Vec2D screenDimensions = Vec2D(1300,800);
const Vec2D screenCenter = Vec2D(650,400);
const double screenViewRadius = sqrt(pow(401,2) + pow(651,2));

//const Vec2D lowOrbitShipViewCenter = Vec2D(650,200);
const Vec2D rotationNormalAxis = Vec2D(0, -1);
const double rotationNormalAxisMag = rotationNormalAxis.magnitude(); // this is just 1
const double stateChangeAltitudeRadiusMult = 1;

/**
 * Converting between radians and degrees
 */
const double radToDeg = 180.0/M_PI;
const double degToRad = 1.0/radToDeg;

template <typename T>
long double clamp(T val, T min, T max) {
    return std::max(min, std::min(max, val));
}

#endif //KSP_2D_CW4_KSP_H
