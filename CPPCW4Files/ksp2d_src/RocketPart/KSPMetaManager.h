//
// Created by Chris Purdy on 09/03/2020.
//

#ifndef KSP_2D_CW4_KSPMETAMANAGER_H
#define KSP_2D_CW4_KSPMETAMANAGER_H

#include <vector>
#include "../../src/ImageManager.h"
#include "Gadget/Gadgets.h"

class RocketPart;

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
    uint32_t fuel;
    uint32_t maxFuel;
    uint32_t monoprop;
    uint32_t maxMonoprop;
    uint32_t electricity;
    uint32_t maxElectricity;
};

struct PartData {
    std::string partName;
    uint32_t health;
    uint32_t maxHealth; // measured in Kelvin
    uint32_t temperature; // measured in Kelvin
    uint32_t maxTemperature; // measured in Kelvin
    AeroBehaviour aerodynamicProfile[4]{}; // each part needs to have a modified aerodynamic profile based on the surrounding parts
    AnchorPoint anchorPoints[4]{};
    Resources resources{};
    std::vector<Gadget*>* gadgets = nullptr;
};

class KSPMetaManager {
public:
    std::map<std::string, std::shared_ptr<PartData> > partDataLookup;

    static KSPMetaManager* get()
    {
        if (singleton == nullptr)
            singleton = new KSPMetaManager();
        return singleton;
    }

private:
    static KSPMetaManager* singleton;

public:

    std::shared_ptr<PartData> getPartData(const std::string& spriteName, bool loadIfNotFound = true);
    PartData* getPartDataCopy(const std::string& spriteName, bool loadIfNotFound = true);

    static Gadget *newGadget(const std::string &gadgetName, const int &gadgetMod);
};

#endif //KSP_2D_CW4_KSPMETAMANAGER_H
