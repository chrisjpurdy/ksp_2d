//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_REACTIONWHEELGADGET_H
#define KSP_2D_CW4_REACTIONWHEELGADGET_H

#include "Gadget.h"

class ReactionWheelGadget : public Gadget {
public:

    explicit ReactionWheelGadget(int gadgetModifier) : Gadget(gadgetModifier, "reactionWheel") {};

};

#endif //KSP_2D_CW4_REACTIONWHEELGADGET_H
