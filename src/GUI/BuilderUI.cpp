//
// Created by Chris Purdy on 20/04/2020.
//

#include "BuilderUI.h"

BuilderUI::BuilderUI(KSP2D* pEngine) : engine(pEngine), objectsAdded(false), shipOffsetCommandNotification(0) {
    shipNameTextbox = new GUITextbox(engine, "SHIP NAME", 30, 100, 40, nullptr);
    partCatalogue = new PartCatalogue(engine, 170, 700, 18);
    makeShipButton = new GUIButton(engine, "CREATE SHIP", 1110, 40, 136, 49, [&]{engine->addNewSpacecraft();});
    saveShipButton = new GUIButton(engine, "SAVE SHIP", 980, 40, 113, 49, [&]{engine->addNewSpacecraft();});
    loadShipButton = new GUIButton(engine, "LOAD SHIP", 850, 40, 113, 49, [&]{engine->addNewSpacecraft();});
}

BuilderUI::~BuilderUI() {
    delete partCatalogue;
    delete shipNameTextbox;
    delete makeShipButton;
    delete loadShipButton;
    delete saveShipButton;
}

void BuilderUI::draw(int iCurrentTime) {
    shipNameTextbox->draw(iCurrentTime);
    engine->getForegroundSurface()->drawFastString(42, 130, "SHIP NAME", 0xffffff, engine->getFont("kenpixel.ttf", 24));
    partCatalogue->draw(iCurrentTime);
    makeShipButton->draw(iCurrentTime);
    loadShipButton->draw(iCurrentTime);
    saveShipButton->draw(iCurrentTime);
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

    if (!(shipOffsetCommandNotification & 1)) {
        GUIManager::get()->fadeTextAlert("Use the arrows to offset the ship view");
        shipOffsetCommandNotification |= 1;
    }
    if ((shipOffsetCommandNotification & 2) && !(shipOffsetCommandNotification & 4)) {
        GUIManager::get()->fadeTextAlert("Press space to centre the ship");
        shipOffsetCommandNotification |= 4;
    }
}

Spacecraft *BuilderUI::makeShip(KSP2D* pEngine, const Vec2D& initalPos, const Vec2D& initialVel, long double mass, Vec2D* origin) {
    return partCatalogue->makeShip(pEngine, initalPos, initialVel, mass, origin);
}
