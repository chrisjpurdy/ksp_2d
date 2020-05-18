//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_MONOPROPTHRUSTERGADGET_H
#define KSP_2D_CW4_MONOPROPTHRUSTERGADGET_H

#include "Gadget.h"

class MonopropThrusterGadget : public Gadget {
public:

    explicit MonopropThrusterGadget(int gadgetModifier) : Gadget(gadgetModifier, "monopropThruster") {};

};

#endif //KSP_2D_CW4_MONOPROPTHRUSTERGADGET_H
