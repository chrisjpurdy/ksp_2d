//
// Created by Chris Purdy on 24/04/2020.
//

#include "SpacecraftParts.h"
#include "../Spacecraft/Spacecraft.h"

SpacecraftParts::SpacecraftParts(int screenX, int screenY, RocketPart* initialPart) {
    parts.push_back(initialPart);
    parts[0]->sprite->setPosition(screenX, screenY);
}

/**
 * Checks if the part can snap to the rocket, at the given x, y coords, abd if so will connect the part
 *
 * @param x         mouse x
 * @param y         mouse y
 * @param toSnap    part being dragged by the mouse
 * @return          whether the part was attached or now
 */
std::pair<RocketPart*, RocketPart::Side> SpacecraftParts::getClosestSnapTo(int x, int y, RocketPart* toSnap) {
    // goes through each sprite and checks if the mouse xy is over the top of a possible snap point

    std::pair<RocketPart*, RocketPart::Side> closestAttachment(nullptr, RocketPart::Side::left);
    double closestAttachmentDist = INFINITY;

    // go through parts finding closest attachment point
    for (auto & p : parts) {
        auto nextAttachment = p->closestAttatchTo(x, y);
        if (nextAttachment.first < closestAttachmentDist) {
            if (p->checkAvaliableToAttatch(toSnap, nextAttachment.second)) {
                closestAttachment.first = p;
                closestAttachment.second = nextAttachment.second;
                closestAttachmentDist = nextAttachment.first;
            }
        }
    }

    //std::cout << closestToAttachTo << std::endl;

    return closestAttachment;

}

/**
 * Connects a new part to the given part
 *
 * @param oneToAttach   new part which to being attached
 * @param toConnectTo   part which is being attached to
 * @param snapPoint     indication of which side the new part attaches to
 * @return              whether the part got connected
 */
bool SpacecraftParts::connect(RocketPart* oneToAttach, RocketPart* toConnectTo, RocketPart::Side snapPoint) {
    if (toConnectTo->attachPart(oneToAttach, snapPoint)) {
        parts.push_back(oneToAttach);
        return true;
    }
    return false;
}

/**
 * Checks if the spacecraft is a valid craft (has all types of parts required to launch etc)
 */
bool SpacecraftParts::checkSpacecraftValid() {
    return false;
}

/**
 * Generates a spacecraft from the part contained in this object
 *
 * @return  the newly generated spacecraft
 */
Spacecraft* SpacecraftParts::generateSpacecraft(KSP2D* pEngine, const Vec2D& initalPos, const Vec2D& initialVel, long double mass, Vec2D* origin) {
    if (parts.empty()) {
        return nullptr;
    }
    return new Spacecraft(pEngine, initalPos, initialVel, mass, origin, parts);
}

RocketPart* SpacecraftParts::getDraggedPart() {

    RocketPart* retPart = nullptr;

    bool partsWereDeleted = false;
    for (auto it = parts.begin(); it != parts.end(); it++) {
        auto part = *it;
        if (part->sprite->isBeingDragged()) {
            partsWereDeleted = part->deleteAllSides();
            parts.erase(it);
            retPart = part;
            break;
        }
    }

    // second pass to remove parts which were marked to be deleted
    if (partsWereDeleted) {
        std::vector<RocketPart *> newPartsVec;
        for (auto *part : parts) {
            if (!part->toBeRemoved) {
                newPartsVec.push_back(part);
            } else {
                delete part;
            }
        }
        parts = newPartsVec;
    }

    return retPart;
}

int SpacecraftParts::getNumParts() {
    return parts.size();
}
