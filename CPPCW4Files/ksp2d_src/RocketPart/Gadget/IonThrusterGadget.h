//
// Created by Chris Purdy on 26/04/2020.
//

#ifndef KSP_2D_CW4_IONTHRUSTERGADGET_H
#define KSP_2D_CW4_IONTHRUSTERGADGET_H

#include "Gadget.h"

class IonThrusterGadget : public Gadget {
public:

    explicit IonThrusterGadget(int gadgetModifier) : Gadget(gadgetModifier, "ionThruster") {};

};

#endif //KSP_2D_CW4_IONTHRUSTERGADGET_H
