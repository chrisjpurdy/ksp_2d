//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_MONOPROPTHRUSTERGADGET_H
#define KSP_2D_CW4_MONOPROPTHRUSTERGADGET_H

#include "Gadget.h"

class MonopropThrusterGadget : public Gadget {
public:

    explicit MonopropThrusterGadget(int gadgetModifier) : Gadget(gadgetModifier, "monopropThruster") {};

    void setThrustPercent(double _thrustPercent) {
        thrustPercent = _thrustPercent;
    }

    SpacecraftStateMod& modifySpacecraftState(SpacecraftStateMod& state, long double timeSlice) override {
        state.thrustMod += gadgetModifier * 25000 * timeSlice * thrustPercent;
        state.monoprop += 50.0 * timeSlice;
        return state;
    };

    double thrustPercent = 0;

};

#endif //KSP_2D_CW4_MONOPROPTHRUSTERGADGET_H
