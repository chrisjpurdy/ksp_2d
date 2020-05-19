//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_FUELTHRUSTERGADGET_H
#define KSP_2D_CW4_FUELTHRUSTERGADGET_H

#include "Gadget.h"

class FuelThrusterGadget : public Gadget {
public:

    explicit FuelThrusterGadget(int gadgetModifier) : Gadget(gadgetModifier, "fuelThruster") {};

    void setThrustPercent(double _thrustPercent) {
        thrustPercent = _thrustPercent;
    }

    SpacecraftStateMod& modifySpacecraftState(SpacecraftStateMod& state, long double timeSlice) override {
        state.thrustMod += gadgetModifier * 30000 * timeSlice * thrustPercent;
        state.fuel += 70.0 * timeSlice;
        //std::cout << "Modified state with thrust: " << gadgetModifier * 70000 * timeSlice * thrustPercent << std::endl;
        return state;
    };

    double thrustPercent = 0;

};

#endif //KSP_2D_CW4_FUELTHRUSTERGADGET_H
