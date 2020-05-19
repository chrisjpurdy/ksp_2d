//
// Created by Chris Purdy on 16/05/2020.
//

#ifndef KSP_2D_CW4_PARTINFOSTRUCTS_H
#define KSP_2D_CW4_PARTINFOSTRUCTS_H

class RocketPart;
class Gadget;

/* Describes the behaviour of each anchor point on the part - how different parts can attach to it,
 * also stores the next part that is attached via this anchor point */
struct AnchorPoint {
    bool block;
    bool radial;
    bool decoupler;
    bool cover;
    uint32_t strength;
    RocketPart* connectedPart;
};

/* Describes aerodynamics of a part from one side */
struct AeroBehaviour {
    uint32_t rating;
    double scaleFactor; // the exponent - 2 means it scales with the speed and atmoshpere squared, 1/2 means square root etc
    // a smaller value gives exponentially less air resistance - parts such as nose cones will have small values of this
};

/* Values of resources this part contains */
struct Resources {
    float fuel;
    float maxFuel;
    float monoprop;
    float maxMonoprop;
    float electricity;
    float maxElectricity;
};

struct PartData {
    std::string partName;
    std::string spriteName;
    uint32_t health;
    float mass;
    uint32_t maxHealth; // measured in Kelvin
    uint32_t temperature; // measured in Kelvin
    uint32_t maxTemperature; // measured in Kelvin
    AeroBehaviour aerodynamicProfile[4]{}; // each part needs to have a modified aerodynamic profile based on the surrounding parts
    AnchorPoint anchorPoints[4]{};
    Resources resources{};
    std::vector<Gadget*>* gadgets = nullptr;
};

#endif //KSP_2D_CW4_PARTINFOSTRUCTS_H
