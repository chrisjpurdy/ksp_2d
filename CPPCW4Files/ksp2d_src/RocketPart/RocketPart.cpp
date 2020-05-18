//
// Created by Chris Purdy on 08/03/2020.
//

#include "RocketPart.h"

/**
 *
 * @param attachee  other part to be attached to this one
 * @param fromSide  from which side to attach it from
 * @return          if the part was attached
 */
bool RocketPart::attachPart(RocketPart* attachee, Side fromSide) {
    if (checkAvaliableToAttatch(attachee, fromSide)) {
        partData->anchorPoints[fromSide].connectedPart = attachee;
        attachee->partData->anchorPoints[oppositeSide(fromSide)].connectedPart = this;
        int anchorX = sprite->getXCentre();
        int anchorY = sprite->getYCentre();
        modifyCentreToAnchorPoint(anchorX, anchorY, fromSide);
        attachee->modifyCentreToAnchorPoint(anchorX, anchorY, fromSide);
        attachee->sprite->setPosition(anchorX - attachee->sprite->getDrawWidth() / 2,
                                      anchorY - attachee->sprite->getDrawHeight() / 2);
        return true;
    }
    return false;
}

/**
 *
 * @param attachee  other part to be attached to this one
 * @param fromSide  from which side to attach it from
 * @return          if the part can be attached
 */
bool RocketPart::checkAvaliableToAttatch(RocketPart* attachee, Side fromSide) {
    //std::cout << "Attempting to attach part" << std::endl;

    Side attacheeSnapPoint = oppositeSide(fromSide);

    // if at least one type of attach point don't match up for the given attach points
    // TODO if the attach points are both decoupler points, one of the parts must be a decoupler
    if ((attachee->partData->anchorPoints[attacheeSnapPoint].block && partData->anchorPoints[fromSide].block) ||
        (attachee->partData->anchorPoints[attacheeSnapPoint].radial && partData->anchorPoints[fromSide].radial) ||
        (attachee->partData->anchorPoints[attacheeSnapPoint].cover && partData->anchorPoints[fromSide].cover) ||
        (attachee->partData->anchorPoints[attacheeSnapPoint].decoupler && partData->anchorPoints[fromSide].decoupler)) {

        // if the part doesn't already have a connected part at this point
        if (!partData->anchorPoints[fromSide].connectedPart) {

            //std::cout << "Attached from side: " << fromSide << std::endl;
            return true;
        }
    }
    //std::cout << "Could not attach part from side: " << fromSide << std::endl;
    return false;
}

/**
 * Returns the closest attachment point on this part to the given xy, with its distance from the
 * point given SQUARED
 *
 * @param x     mouse x
 * @param y     mouse y
 * @return      pair containing dist between drag point and attach point SQUARED
 */
std::pair<double, RocketPart::Side> RocketPart::closestAttatchTo(int x, int y) {

    std::pair<double, RocketPart::Side> ret(INFINITY, left);

    for (int i=0; i<4; i++) {
        if (!partData->anchorPoints[Side(i)].connectedPart) {
            int anchorX = sprite->getXCentre();
            int anchorY = sprite->getYCentre();
            modifyCentreToAnchorPoint(anchorX, anchorY, Side(i));
            double dist = pow(anchorX - x, 2) + pow(anchorY - y, 2);
            if (dist < ret.first) {
                ret = {dist, Side(i)};
            }
        }
    }

    return ret;
}

RocketPart::RocketPart(PartSprite* _sprite) {
    sprite = _sprite;
    partData = KSPMetaManager::get()->getPartDataCopy(sprite->data->name, true);
    for (auto & anchorPoint : partData->anchorPoints) {
        anchorPoint.connectedPart = nullptr;
    }
}

bool RocketPart::deleteAllSides() {
    bool partsWereDeleted = false;
    for (int i=0; i<4; i++) {
        if (partData->anchorPoints[i].connectedPart) {
            partData->anchorPoints[i].connectedPart->partData->anchorPoints[oppositeSide(Side(i))].connectedPart = nullptr;
            if (!partData->anchorPoints[i].connectedPart->checkIfConnectedToRoot(this)) {
                if (!partData->anchorPoints[i].connectedPart->toBeRemoved) partData->anchorPoints[i].connectedPart->recursivelyDelete();
                partsWereDeleted = true;
            }
        }
        partData->anchorPoints[i].connectedPart = nullptr;
    }
    return partsWereDeleted;
}

void RocketPart::recursivelyDelete() {
    toBeRemoved = true;
    for (auto & anchorPoint : partData->anchorPoints) {
        if (anchorPoint.connectedPart) {
            if (!anchorPoint.connectedPart->toBeRemoved) anchorPoint.connectedPart->recursivelyDelete();
        }
    }
}

/**
 * Before a part is deleted all of the parts around it are told about it by recursivelyDelete() and deleteAllSides() (so those
 * should be used to delete)
 */
RocketPart::~RocketPart() {
    for (int i = partData->gadgets->size() - 1; i >= 0; i--) {
        delete partData->gadgets->at(i);
    }
    delete partData->gadgets;
    delete partData;
    delete sprite;
}

bool RocketPart::checkIfConnectedToRoot(RocketPart* prev) {
    if (isRootPart) return true;

    for (auto & anchorPoint : partData->anchorPoints) {
        if (anchorPoint.connectedPart) {
            if (anchorPoint.connectedPart != prev) {
                if (anchorPoint.connectedPart->checkIfConnectedToRoot(this)) return true;
            }
        }
    }
    return false;
}



