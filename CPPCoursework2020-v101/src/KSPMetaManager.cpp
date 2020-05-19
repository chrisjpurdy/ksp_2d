//
// Created by Chris Purdy on 25/04/2020.
//

#include "KSPMetaManager.h"

KSPMetaManager* KSPMetaManager::singleton = nullptr;

std::shared_ptr<PartData> KSPMetaManager::getPartData(const std::string& spriteName, bool loadIfNotFound) {

    auto iter = partDataLookup.find(spriteName);
    if (iter != partDataLookup.end()) {
        return iter->second;
    }

    if (!loadIfNotFound) {
        std::cout << "KSPMetaManager::getPartData() : Couldn't find data for sprite " << spriteName << std::endl;
        return nullptr;
    }

    //std::cout << "\t\t-*- Finding part data for sprite: " << spriteName << std::endl;

    FILE* fp = fopen("parts.json", "r");

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document json;
    json.ParseStream(is);

    fclose(fp);

    for (auto& v : json.GetArray()) {
        if (strcmp(v["sprite"].GetString(), spriteName.c_str()) == 0) {
            //std::cout << "\t\t-*- Found part data for sprite: " << spriteName << std::endl;
            auto* pd = new PartData;
            pd->partName = v["name"].GetString();
            pd->spriteName = v["sprite"].GetString();
            pd->health = v["health"].GetUint();
            pd->mass = v["mass"].GetFloat();
            pd->maxHealth = v["health"].GetUint();
            pd->maxTemperature = v["max_temp"].GetUint();
            pd->resources.fuel = pd->resources.maxFuel = v["resources"]["fuel"].GetUint();
            pd->resources.monoprop = pd->resources.maxMonoprop = v["resources"]["monoprop"].GetUint();
            pd->resources.electricity = pd->resources.maxElectricity = v["resources"]["electricity"].GetUint();
            for (int i=0; i<4; i++) {
                pd->anchorPoints[i].block = v["anchors"][i]["block"].GetBool();
                pd->anchorPoints[i].radial = v["anchors"][i]["radial"].GetBool();
                pd->anchorPoints[i].decoupler = v["anchors"][i]["decoupler"].GetBool();
                pd->anchorPoints[i].cover = v["anchors"][i]["cover"].GetBool();
                pd->anchorPoints[i].strength = v["anchors"][i]["strength"].GetUint();
                pd->anchorPoints[i].connectedPart = nullptr;
                pd->aerodynamicProfile[i].rating = v["aero"][i]["rating"].GetUint();
                pd->aerodynamicProfile[i].scaleFactor = v["aero"][i]["scaling"].GetDouble();
            }
            pd->temperature = 21;

            // Populates the array of gadgets
            pd->gadgets = new std::vector<Gadget*>();
            auto gadgets = v["gadgets"].GetArray();
            auto gadgetMods = v["gadgetMod"].GetArray();

            for (unsigned int i=0; i < gadgets.Size(); i++) {
                pd->gadgets->push_back(newGadget(gadgets[i].GetString(), gadgetMods[i].GetInt()));
            }

            std::shared_ptr<PartData> sharedPtr(pd);

            partDataLookup.insert(std::pair<std::string, std::shared_ptr<PartData>>(spriteName, sharedPtr) );
            return sharedPtr;
        }
    }
    std::cout << "KSPMetaManager::getPartData() : Couldn't load data for sprite " << spriteName << std::endl;
    return nullptr; // couldn't find the image

}

PartData* KSPMetaManager::getPartDataCopy(const std::string &spriteName, bool loadIfNotFound) {
    auto sharedPartPtr = getPartData(spriteName, loadIfNotFound);
    if (!sharedPartPtr) {
        return nullptr;
    } else {
        auto* ret = new PartData();
        *ret = *sharedPartPtr;
        ret->gadgets = new std::vector<Gadget*>();
        for (auto & g : *sharedPartPtr->gadgets) {
            ret->gadgets->push_back(newGadget(g->gadgetName, g->gadgetModifier));
        }
        return ret;
    }
}

Gadget* KSPMetaManager::newGadget(const std::string& gadgetName, const int& gadgetMod) {
    if (gadgetName == "solarPanel") {
        return new SolarPanelGadget(gadgetMod);
    } else if (gadgetName == "reactionWheel") {
        return new ReactionWheelGadget(gadgetMod);
    } else if (gadgetName == "decoupler") {
        return new DecouplerGadget(gadgetMod);
    } else if (gadgetName == "controller") {
        return new ControllerGadget(gadgetMod);
    } else if (gadgetName == "antenna") {
        return new AntennaGadget(gadgetMod);
    } else if (gadgetName == "fuelThruster") {
        return new FuelThrusterGadget(gadgetMod);
    } else if (gadgetName == "monopropThruster") {
        return new MonopropThrusterGadget(gadgetMod);
    } else if (gadgetName == "ionThruster") {
        return new IonThrusterGadget(gadgetMod);
    } else if (gadgetName == "crewContainer") {
        return new CrewContainerGadget(gadgetMod);
    }
    std::cout << "KSPMetaManager::newGadget() : Gadget with name: " << gadgetName << " doesn't exist" << std::endl;
    return nullptr;
}
