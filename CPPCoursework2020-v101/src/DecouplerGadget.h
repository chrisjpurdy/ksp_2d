//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_DECOUPLERGADGET_H
#define KSP_2D_CW4_DECOUPLERGADGET_H

#include "Gadget.h"

class DecouplerGadget : public Gadget {
public:

    explicit DecouplerGadget(int gadgetModifier) : Gadget(gadgetModifier, "decoupler") {};

};

#endif //KSP_2D_CW4_DECOUPLERGADGET_H
