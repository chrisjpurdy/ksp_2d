//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_CREWCONTAINERGADGET_H
#define KSP_2D_CW4_CREWCONTAINERGADGET_H

#include "Gadget.h"

class CrewContainerGadget : public Gadget {
public:

    explicit CrewContainerGadget(int gadgetModifier) : Gadget(gadgetModifier, "crewContainer") {};

};

#endif //KSP_2D_CW4_CREWCONTAINERGADGET_H
