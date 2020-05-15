//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_SOLARPANELGADGET_H
#define KSP_2D_CW4_SOLARPANELGADGET_H

#include "Gadget.h"

class SolarPanelGadget : public Gadget {
public:

    explicit SolarPanelGadget(int gadgetModifier) : Gadget(gadgetModifier, "solarPanel") {};

};

#endif //KSP_2D_CW4_SOLARPANELGADGET_H
