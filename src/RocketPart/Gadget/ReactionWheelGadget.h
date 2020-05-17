//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_REACTIONWHEELGADGET_H
#define KSP_2D_CW4_REACTIONWHEELGADGET_H

#include "Gadget.h"

class ReactionWheelGadget : public Gadget {
public:

    explicit ReactionWheelGadget(int gadgetModifier) : Gadget(gadgetModifier, "reactionWheel") {};

    SpacecraftStateMod& modifySpacecraftState(SpacecraftStateMod& state, long double timeSlice) override {
        // TODO NEEDS TUNING TO BE GOOD VALUES
        state.rotationMod += gadgetModifier * 0.05 * timeSlice * direction;
        state.elec += 10.0 * timeSlice;
        return state;
    };

    void setRotateDirection(int d) {
        direction = d;
    }

    int direction = 1;

};

#endif //KSP_2D_CW4_REACTIONWHEELGADGET_H
