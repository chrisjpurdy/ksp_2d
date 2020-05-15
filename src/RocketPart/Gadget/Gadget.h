//
// Created by Chris Purdy on 08/03/2020.
//

#ifndef KSP_2D_CW4_GADGET_H
#define KSP_2D_CW4_GADGET_H

#include "../../PhysUtils/Vec2D.h"

/**
 * this gets passed around the rocket parts to accumulate changes to the spacecraft's state
 */
struct SpacecraftStateMod {
    Vec2D thrustMod;
    Vec2D rotThrustMod;
    uint8_t fuelMod;
    uint8_t elecMod;
    uint8_t scienceMod;
};

class Gadget {
public:
    /**
     * Activates the gadget (for example opens a parachute or starts a solid fuel booster)
     *
     * @return  whether the part was activated
     */
    virtual bool activate() { return false; };

    /**
     * Deactivates the gadget (for example stops a fuel based thruster)
     *
     * @return  whether the part was deactivated
     */
    virtual bool disable() { return false; };

    /**
     * Checks whether the gadget is a gadget which is inside a part which can act as a basis for a spacecraft (a drone core or a command pod)
     * returns false unless overridden
     *
     * @return  whether the gadget is a controllable gadget
     */
    virtual bool isControllable() { return false; };

    /**
     * Takes a description of the spacecrafts state and modifies it in some way
     *
     * @param   state
     * @return  the same state object but modified
     */
    virtual SpacecraftStateMod& modifySpacecraftState(SpacecraftStateMod& state) {
        return state;
    };

    explicit Gadget(int _gadgetModifier, const char* name) : activated(false), gadgetModifier(_gadgetModifier), gadgetName(name) {};

    bool activated;
    int gadgetModifier;
    std::string gadgetName;

};

#endif //KSP_2D_CW4_GADGET_H
