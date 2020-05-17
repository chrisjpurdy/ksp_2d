//
// Created by Chris Purdy on 08/03/2020.
//

#ifndef KSP_2D_CW4_ROCKETPART_H
#define KSP_2D_CW4_ROCKETPART_H

#include "PartSprite.h"
#include "KSPMetaManager.h"

class SpacecraftParts;

class RocketPart {
public:

    explicit RocketPart(PartSprite *_sprite);

    ~RocketPart();

    enum Side {left = 0, right = 1, top = 2, bottom = 3};

    friend std::ostream & operator<<(std::ostream & out, const Side &s) {
        switch (s) {
            case left:
                out << "left";
                break;
            case right:
                out << "right";
                break;
            case top:
                out << "top";
                break;
            case bottom:
                out << "bottom";
                break;
        }
        return out;
    }

    static Side oppositeSide(Side s) {
        switch (s) {
            case left:
                return right;
            case right:
                return left;
            case top:
                return bottom;
            case bottom:
                return top;
        }
    }

    void modifyCentreToAnchorPoint(int& x, int& y, Side s) {
        switch (s) {
            case left:
                x -= sprite->getDrawWidth()/2;
                break;
            case right:
                x += sprite->getDrawWidth()/2;
                break;
            case top:
                y -= sprite->getDrawHeight()/2;
                break;
            case bottom:
                y += sprite->getDrawHeight()/2;
                break;
        }
    }

    bool attachPart(RocketPart* attachee, Side fromSide);
    std::pair<double, RocketPart::Side> closestAttatchTo(int x, int y);

    PartData* partData = nullptr;
    PartSprite* sprite = nullptr;

    bool isRootPart = false;
    bool toBeRemoved = false;

    //void disconnectAllSides(); - will be needed for when im doing parts breaking and decoupling etc

    bool deleteAllSides();

    void recursivelyDelete();

    bool checkIfConnectedToRoot(RocketPart *prev);

    bool checkAvaliableToAttatch(RocketPart *attachee, Side fromSide);

    SpacecraftStateMod &modifySpacecraftState(SpacecraftStateMod &state, long double seconds);

    void setThrusterPercent(double thrustPercent);

    bool hasGadget(std::string gadgetName);

    bool isThrusterActivated();

    void drawOnSurface(BaseEngine* engine, DrawingSurface *pSurface, AnimatedSprite *pSprite);

    void setReactionWheelDirection(int direction);
};


#endif //KSP_2D_CW4_ROCKETPART_H
