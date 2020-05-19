//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_IONTHRUSTERGADGET_H
#define KSP_2D_CW4_IONTHRUSTERGADGET_H

#include "Gadget.h"

class IonThrusterGadget : public Gadget {
public:

    explicit IonThrusterGadget(int gadgetModifier) : Gadget(gadgetModifier, "ionThruster") {};

    void setThrustPercent(double _thrustPercent) {
        thrustPercent = _thrustPercent;
    }

    SpacecraftStateMod& modifySpacecraftState(SpacecraftStateMod& state, long double timeSlice) override {
        state.thrustMod += gadgetModifier * 5000 * timeSlice * thrustPercent;
        state.elec += 100.0 * timeSlice;
        return state;
    };

    double thrustPercent = 0;

};

#endif //KSP_2D_CW4_IONTHRUSTERGADGET_H
