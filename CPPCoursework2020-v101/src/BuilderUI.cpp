//
// Created by Chris Purdy on 20/04/2020.
//

#include "BuilderUI.h"

BuilderUI::BuilderUI(KSP2D* pEngine) : engine(pEngine), objectsAdded(false), shipOffsetCommandNotification(0) {
    shipNameTextbox = new GUITextbox(engine, "SHIP NAME", 30, 100, 40, nullptr);
    partCatalogue = new PartCatalogue(engine, 170, 700, 18);
    makeShipButton = new GUIButton(engine, "CREATE SHIP", 1110, 40, 136, 49, [&]{engine->addNewSpacecraft();});
    saveShipButton = new GUIButton(engine, "SAVE SHIP", 980, 40, 113, 49, [&]{this->saveShip();});
    loadShipButton = new GUIButton(engine, "LOAD SHIP", 850, 40, 113, 49, [&]{this->loadShip();});
    shipInfoPanel = new SimpleImage(ImageManager::get()->getSpriteFromImg(ImageManager::get()->getSpriteData("panel_0")));
    shipInfoPanel->setTransparencyColour(0x000000);
}

BuilderUI::~BuilderUI() {
    delete partCatalogue;
    delete shipNameTextbox;
    delete makeShipButton;
    delete loadShipButton;
    delete saveShipButton;
    delete shipInfoPanel;
}

void BuilderUI::draw(int iCurrentTime) {
    shipNameTextbox->draw(iCurrentTime);
    engine->getForegroundSurface()->drawFastString(42, 130, "SHIP NAME", 0xffffff, engine->getFont("kenpixel.ttf", 24));
    partCatalogue->draw(iCurrentTime);
    makeShipButton->draw(iCurrentTime);
    loadShipButton->draw(iCurrentTime);
    saveShipButton->draw(iCurrentTime);
    drawShipInfo(engine->getForegroundSurface());
}

void BuilderUI::drawShipInfo(DrawingSurface* surface) {
    if (partCatalogue->spacecraftParts) {
        int topX = 950, topY = 100;
        shipInfoPanel->renderImageBlit(engine, surface, topX, topY, shipInfoPanel->getWidth() * 3,
                                       shipInfoPanel->getHeight() * 3,
                                       0, 0, shipInfoPanel->getWidth(), shipInfoPanel->getHeight());

        float mass = 0;
        uint32_t fuel = 0, mono = 0, elec = 0;

        for (auto part : partCatalogue->spacecraftParts->parts) {
            mass += part->partData->mass;
            fuel += part->partData->resources.maxFuel;
            mono += part->partData->resources.maxMonoprop;
            elec += part->partData->resources.maxElectricity;
        }
        surface->drawFastString(topX + 22, topY + 20, "Ship data", 0xffffff, engine->getFont("kenpixel.ttf", 16));
        surface->drawFastString(topX + 22, topY + 45, (std::string("Mass (t): ") + std::to_string(mass)).c_str(), 0xffffff, engine->getFont("kenpixel.ttf", 16));
        surface->drawFastString(topX + 22, topY + 70, "Resources: ", 0xffffff, engine->getFont("kenpixel.ttf", 16));
        surface->drawFastString(topX + 32, topY + 95, (std::string("- Fuel: ") + std::to_string(fuel)).c_str(), 0xffffff, engine->getFont("kenpixel.ttf", 16));
        surface->drawFastString(topX + 32, topY + 120, (std::string("- Monopropellant: ") + std::to_string(mono)).c_str(), 0xffffff, engine->getFont("kenpixel.ttf", 16));
        surface->drawFastString(topX + 32, topY + 145, (std::string("- Electricity: ") + std::to_string(elec)).c_str(), 0xffffff, engine->getFont("kenpixel.ttf", 16));
    }
}

void BuilderUI::addObjectsToEngine() {
    if (objectsAdded) return;
    engine->appendObjectToArray(shipNameTextbox);
    engine->appendObjectToArray(partCatalogue);
    engine->appendObjectToArray(makeShipButton);
    engine->appendObjectToArray(loadShipButton);
    engine->appendObjectToArray(saveShipButton);
    objectsAdded = true;
}

void BuilderUI::setVisible(bool visible) {
    partCatalogue->setVisible(visible);
    shipNameTextbox->setVisible(visible);
    makeShipButton->setVisible(visible);
    loadShipButton->setVisible(visible);
    saveShipButton->setVisible(visible);
}

void BuilderUI::update(int iCurrentTime) {

    if (engine->state != KSP2D::State::stateBuilder) return;

    if (engine->isKeyListener) {
        // should handle any GUI animations etc
        if (engine->isKeyPressed(SDLK_LEFT)) {
            partCatalogue->offsetShip(-0.01, 0);
            shipOffsetCommandNotification |= 2;
        }
        if (engine->isKeyPressed(SDLK_DOWN)) {
            partCatalogue->offsetShip(0, 0.01);
            shipOffsetCommandNotification |= 2;
        }
        if (engine->isKeyPressed(SDLK_UP)) {
            partCatalogue->offsetShip(0, -0.01);
            shipOffsetCommandNotification |= 2;
        }
        if (engine->isKeyPressed(SDLK_RIGHT)) {
            partCatalogue->offsetShip(0.01, 0);
            shipOffsetCommandNotification |= 2;
        }
        if (engine->isKeyPressed(SDLK_SPACE)) {
            partCatalogue->resetShipOffset();
        }
    }

    if (!(shipOffsetCommandNotification & 1)) {
        GUIManager::get()->fadeTextAlert("Use the arrows to offset the ship view");
        shipOffsetCommandNotification |= 1;
    }
    if ((shipOffsetCommandNotification & 2) && !(shipOffsetCommandNotification & 4)) {
        GUIManager::get()->fadeTextAlert("Press space to centre the ship");
        shipOffsetCommandNotification |= 4;
    }
}

Spacecraft *BuilderUI::makeShip(KSP2D* pEngine, const Vec2D& initalPos, const Vec2D& initialVel, Vec2D* origin) {
    Spacecraft* ret = partCatalogue->makeShip(pEngine, initalPos, initialVel, origin);
    if (ret) ret->shipName = shipNameTextbox->value;
    return ret;
}

/**
 * Saves the ship to a json file
 */
void BuilderUI::saveShip() {

    if (!partCatalogue->spacecraftParts) {
        GUIManager::get()->fadeTextAlert("Please create a spacecraft to save it");
    }

    std::ofstream fp;
    fp.open("ship.json");
    fp << "{\n\"shipName\":" << "\"" << shipNameTextbox->value << "\",\n";
    fp << "\"ship\":" << stringifyPartData(partCatalogue->spacecraftParts->parts[0]->partData, 0) << "\n}";
    fp.close();

    GUIManager::get()->fadeTextAlert("Saved ship to file");

}

std::string BuilderUI::stringifyPartData(PartData* data, int sideToIgnore = -1) {
    std::stringstream partDataJson;
    partDataJson << "{\n\"name\": " << "\"" << data->spriteName << "\",\n";
    partDataJson << "\"connectedParts\": [";

    std::string parts;
    for (int i=0; i<4; i++) {
        if (data->anchorPoints[i].connectedPart && i != sideToIgnore) {
            parts += "\n{\n";
            parts += "\"side\": ";
            parts += std::to_string(i);
            parts += ",\n";
            parts += "\"part\": " + stringifyPartData(data->anchorPoints[i].connectedPart->partData, RocketPart::oppositeSide(RocketPart::Side(i))) + "\n";
            parts += "},";
        }
    }
    if (!parts.empty()) { parts.pop_back();
    partDataJson << parts << "\n]}"; }
    return partDataJson.str();
}

/**
 * Loads the ship stored in a json file
 */
void BuilderUI::loadShip() {

    FILE* fp = fopen("ship.json", "r");

    if (fp == nullptr) {
        GUIManager::get()->fadeTextAlert("Error loading ship - file doesn't exist");
        return;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document json;
    json.ParseStream(is);

    auto* newPartSprite = new PartSprite(json["ship"]["name"].GetString(), engine, 0, 0);
    engine->appendObjectToArray(newPartSprite);
    auto* rootPart = new RocketPart(newPartSprite);
    std::vector<RocketPart*> sideMostParts;
    for (int i=0; i<4; i++) sideMostParts.push_back(rootPart);
    rootPart->isRootPart = true;
    rootPart->sprite->locked = true;

    auto* spacecraftParts = new SpacecraftParts(screenCenter.x, screenCenter.y, rootPart);

    bool ret = true;
    for (auto& connectedPart : json["ship"]["connectedParts"].GetArray()) {
        ret = ret && recursivelyAddParts(connectedPart["part"], RocketPart::Side(connectedPart["side"].GetInt()), rootPart,
                            spacecraftParts, sideMostParts);
    }

    if (!ret) {
        GUIManager::get()->fadeTextAlert("Error loading ship - corrupted save file");
    } else {
        if (partCatalogue->spacecraftParts) {
            for (auto* part : partCatalogue->spacecraftParts->parts) {
                delete part;
            }
            delete partCatalogue->spacecraftParts;
        }
        partCatalogue->spacecraftParts = spacecraftParts;
        partCatalogue->sideMostParts = sideMostParts;
        shipNameTextbox->value = json["shipName"].GetString();
        GUIManager::get()->fadeTextAlert("Loaded ship from file");
    }

	fclose(fp);
}

bool BuilderUI::recursivelyAddParts(rapidjson::Value &part, RocketPart::Side side, RocketPart *parentPart,
                                    SpacecraftParts *spacecraftParts, std::vector<RocketPart *>& sideMostParts) {

    auto partObject = part.GetObject();

    auto* newPartSprite = new PartSprite(partObject["name"].GetString(), engine, 0, 0);
    engine->appendObjectToArray(newPartSprite);
    auto* newPart = new RocketPart(newPartSprite);
    newPart->sprite->locked = true;

    if (spacecraftParts->connect(newPart, parentPart, side)) {
        if (spacecraftParts->isMoreSideMost(newPart, side, sideMostParts)) {
            sideMostParts[side] = newPart;
            spacecraftParts->reposParts(newPartSprite->getDrawWidth() / 2,
                                        newPartSprite->getDrawHeight() / 2, side,
                                        sideMostParts);
        }
    } else {
        // if the part can't connect something went wrong
        return false;
    }

    bool ret = true;
    for (auto& connectedPart : partObject["connectedParts"].GetArray()) {
        ret = ret && recursivelyAddParts(connectedPart["part"], RocketPart::Side(connectedPart["side"].GetInt()), newPart,
                            spacecraftParts, sideMostParts);
    }
    return ret;

}