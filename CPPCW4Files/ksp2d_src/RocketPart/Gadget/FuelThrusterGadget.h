//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_FUELTHRUSTERGADGET_H
#define KSP_2D_CW4_FUELTHRUSTERGADGET_H

#include "Gadget.h"

class FuelThrusterGadget : public Gadget {
public:

    explicit FuelThrusterGadget(int gadgetModifier) : Gadget(gadgetModifier, "fuelThruster") {};

};

#endif //KSP_2D_CW4_FUELTHRUSTERGADGET_H
