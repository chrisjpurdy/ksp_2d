//
// Created by Chris Purdy on 24/04/2020.
//

#ifndef KSP_2D_CW4_SPACECRAFTPARTS_H
#define KSP_2D_CW4_SPACECRAFTPARTS_H

#include <vector>
#include "../PhysUtils/FastVec2D.h"
#include "RocketPart.h"
#include "../KSP2D.h"

class Spacecraft;

/**
 * Container to manage amalgamation of rocket parts to build a rocket from
 */
class SpacecraftParts {
public:

    SpacecraftParts(int screenX, int screenY, RocketPart* initialPart);

    /**
     * Connects a new part to the given part
     *
     * @param oneToAttach   new part which to being attached
     * @param toConnectTo   part which is being attached to
     * @param snapPoint     indication of which side the new part attaches to
     * @return              whether the part got connected
     */
    bool connect(RocketPart *oneToAttach, RocketPart *toConnectTo, RocketPart::Side snapPoint);

    /**
     * Checks if the spacecraft is a valid craft (has all types of parts required in the correct places etc)
     */
    bool checkSpacecraftValid();

    /**
     * Generates a spacecraft from the part contained in this object
     *
     * @return  the newly generated spacecraft
     */
    Spacecraft* generateSpacecraft(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, long double mass,
                                   Vec2D *origin);

    std::vector<RocketPart*> parts;

    RocketPart* getDraggedPart();

    int getNumParts();

    /**
     *
     * @param x         mouse x
     * @param y         mouse y
     * @param toSnap    part being dragged by the mouse
     * @return          the closest snap point, and the side to snap to
     */
    std::pair<RocketPart*, RocketPart::Side> getClosestSnapTo(int x, int y, RocketPart *toSnap);
};

#endif //KSP_2D_CW4_SPACECRAFTPARTS_H
