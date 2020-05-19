//
// Created by Chris Purdy on 09/03/2020.
//

#ifndef KSP_2D_CW4_KSPMETAMANAGER_H
#define KSP_2D_CW4_KSPMETAMANAGER_H

#include <vector>
#include "../../sdl2_framework/ImageManager.h"
#include "Gadget/Gadgets.h"
#include "PartInfoStructs.h"

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
