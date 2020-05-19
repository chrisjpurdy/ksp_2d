//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_CONTROLLERGADGET_H
#define KSP_2D_CW4_CONTROLLERGADGET_H

#include "Gadget.h"

class ControllerGadget : public Gadget {
public:

    explicit ControllerGadget(int gadgetModifier) : Gadget(gadgetModifier, "controller") {};

    bool isControllable() override { return true; };

};

#endif //KSP_2D_CW4_CONTROLLERGADGET_H
