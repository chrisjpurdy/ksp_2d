//
// Created by Chris Purdy on 20/04/2020.
//

#include "BuilderUI.h"

BuilderUI::BuilderUI(KSP2D* pEngine) : engine(pEngine), objectsAdded(false) {
    shipNameTextbox = new GUITextbox(engine, "SHIP NAME", 30, 100, 40, nullptr);
    partCatalogue = new PartCatalogue(engine, 170, 610, 18);
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
    // should handle any GUI animations etc
}

Spacecraft *BuilderUI::makeShip(KSP2D* pEngine, const Vec2D& initalPos, const Vec2D& initialVel, long double mass, Vec2D* origin) {
    return partCatalogue->makeShip(pEngine, initalPos, initialVel, mass, origin);
}
