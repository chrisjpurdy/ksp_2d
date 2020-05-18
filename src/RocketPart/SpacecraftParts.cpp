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
Spacecraft* SpacecraftParts::generateSpacecraft(KSP2D* pEngine, const Vec2D& initalPos, const Vec2D& initialVel,
                                                int width, int height, Vec2D* origin, const Vec2D& tl, const Vec2D& br) {
    if (parts.empty()) {
        return nullptr;
    }
    return new Spacecraft(pEngine, initalPos, initialVel, width, height, origin, parts, tl, br);
}

/**
 * Gets the part being dragged on the ship, and deletes parts that it was attached to which aren't
 * attached to the root part in some way
 *
 * @return      the part being dragged off of the ship, if there is one (nullptr otherwise)
 */
RocketPart* SpacecraftParts::getDraggedPart(std::vector<RocketPart*>& sideMostParts) {

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

    int numParts = getNumParts() + 1;
    int initAvX, initAvY;

    // second pass to remove parts which were still marked to be deleted
    if (partsWereDeleted) {
        // make sure to take into account the removed part
        initAvX = retPart->sprite->getXCentre();
        initAvY = retPart->sprite->getYCentre();

        std::vector<RocketPart *> newPartsVec;
        for (auto* part : parts) {
            initAvX += part->sprite->getXCentre();
            initAvY += part->sprite->getYCentre();
            if (!part->toBeRemoved) {
                newPartsVec.push_back(part);
            } else {
                delete part;
            }
        }
        initAvX /= numParts;
        initAvY /= numParts;
        parts = newPartsVec;
    }

    if (retPart) {
        if (partsWereDeleted) {
            //refresh the side most parts completely
            for (int i=0; i<4; i++) {
                sideMostParts[i] = nullptr;
                findMostSideMost(RocketPart::Side(i), sideMostParts);
            }
            // recalc the centre of the ship and repos parts based on previous centre
            numParts = getNumParts();
            if (numParts > 0) {
                int avX, avY;
                for (auto part : parts) {
                    avX += part->sprite->getXCentre();
                    avY += part->sprite->getYCentre();
                }
                avX /= numParts;
                avY /= numParts;
                for (auto part : parts) {
                    part->sprite->movePosition(initAvX - avX, initAvY - avY);
                }
            }
        } else {
            for (int i=0; i<4; i++) {
                if (sideMostParts[i] == retPart) {
                    sideMostParts[i] = nullptr;
                    reposParts(retPart->sprite->getDrawWidth()/2, retPart->sprite->getDrawHeight()/2,
                            RocketPart::oppositeSide(RocketPart::Side(i)), sideMostParts);
                }
                findMostSideMost(RocketPart::Side(i), sideMostParts);
            }
        }
    }

    return retPart;
}

bool SpacecraftParts::isMoreSideMost(RocketPart* newPart, RocketPart::Side side, std::vector<RocketPart*>& sideMostParts) {

    RocketPart* sideMost = sideMostParts[side];
    int sideMostX = sideMost->sprite->getXCentre();
    int sideMostY = sideMost->sprite->getYCentre();
    int newPartX = newPart->sprite->getXCentre();
    int newPartY = newPart->sprite->getYCentre();

    sideMost->modifyCentreToAnchorPoint(sideMostX, sideMostY, side);
    newPart->modifyCentreToAnchorPoint(newPartX, newPartY, side);

    switch (side) {
        case RocketPart::Side::left:
            if (newPartX < sideMostX) return true;
            break;
        case RocketPart::Side::top:
            if (newPartY < sideMostY) return true;
            break;
        case RocketPart::Side::right:
            if (newPartX > sideMostX) return true;
            break;
        case RocketPart::Side::bottom:
            if (newPartY > sideMostY) return true;
            break;
    }
    return false;
}

void SpacecraftParts::findMostSideMost(RocketPart::Side side, std::vector<RocketPart*>& sideMostParts) {

    if (parts.empty()) {
        for (int i=0; i<4; i++) sideMostParts[i] = nullptr;
        return;
    }

    sideMostParts[side] = parts[0];
    for (auto part : parts) {
        if (part != parts[0]) {
            if (isMoreSideMost(part, side, sideMostParts)) sideMostParts[side] = part;
        }
    }
}

void SpacecraftParts::reposParts(int xChangeAbs, int yChangeAbs, RocketPart::Side side, std::vector<RocketPart*>& sideMostParts) {

    maskReposWithSide(xChangeAbs, yChangeAbs, side);

    for (auto part : parts) {
        if (part) part->sprite->movePosition(xChangeAbs, yChangeAbs);
    }
}

void SpacecraftParts::maskReposWithSide(int& x, int& y, RocketPart::Side side) {
    switch (side) {
        case RocketPart::Side::left:
            y = 0;
            break;
        case RocketPart::Side::top:
            x = 0;
            break;
        case RocketPart::Side::right:
            x *= -1;
            y = 0;
            break;
        case RocketPart::Side::bottom:
            y *= -1;
            x = 0;
            break;
    }
}

int SpacecraftParts::getNumParts() {
    return parts.size();
}

SpacecraftParts::~SpacecraftParts() = default;
