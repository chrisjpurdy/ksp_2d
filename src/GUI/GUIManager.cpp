//
// Created by Chris Purdy on 18/03/2020.
//

#include "GUIManager.h"
#include "BuilderUI.h"

GUIManager* GUIManager::singleton = nullptr;

GUIManager::GUIManager(KSP2D *pEngine) : fadeText(""), spacecraftControls(nullptr) {
    engine = pEngine;
    fadeTextTimer = 0; fadeTextMaxTimer = 400;
    planetLabelsTimer = 0; planetLabelsMaxTimer = 300;
    builderUI = new BuilderUI(engine);
}

GUIManager::~GUIManager() {
    delete spacecraftControls;
    delete builderUI;
}

void GUIManager::draw(int time, KSP2D::State state) {
    drawFadeText();
    drawOrbitGUI();
    if (state == KSP2D::State::stateFlight || state == KSP2D::State::stateLowOrbit ||
                state == KSP2D::State::stateReentry || state == KSP2D::State::stateTakeoff || state == KSP2D::State::stateOrbit) {
        drawSpacecraftGUI(time);
    }
    if (state == KSP2D::State::stateBuilder) {
        drawBuilderGUI(time);
    }
}

void GUIManager::update(int time) {
    if (spacecraftControls) spacecraftControls->update(time);
    if (builderUI) builderUI->update(time);
}

void GUIManager::addObjectsToEngine() {
    if (spacecraftControls) spacecraftControls->addObjectsToEngine();
    if (builderUI) builderUI->addObjectsToEngine();
}

/* Text alert functions */

void GUIManager::fadeTextAlert(const char* text) {
    fadeText = text;
    fadeTextTimer = fadeTextMaxTimer;
}

void GUIManager::drawFadeText() {
    if(fadeTextTimer > 0) {
        char* fadeCStr = new char[fadeText.size() + 1];
        memcpy(fadeCStr, (void *) fadeText.c_str(), fadeText.size() + 1);

        fadeCStr[std::min((int)fadeText.size(),(int)((fadeText.size() * 2) * (1 - abs((fadeTextMaxTimer * 0.5) - fadeTextTimer)/(fadeTextMaxTimer * 0.5))))] = '\0';

        engine->getForegroundSurface()->drawFastString(40, 50, fadeCStr, 0xffffff);
        --fadeTextTimer;

        free(fadeCStr);
    }
}

/* Orbit GUI functions */

void GUIManager::showFadePlanetLabels() {
    planetLabelsTimer = planetLabelsMaxTimer;
}

void GUIManager::drawOrbitGUI() {
    if (planetLabelsTimer > 0) {
        for (auto* p : engine->planets) {
            engine->getForegroundSurface()->drawFastString(p->getXCentre() + 5, p->getYCentre() + 5, p->name.c_str(), 0xffffff, engine->getFont("kenpixel.ttf", 16));
        }
        --planetLabelsTimer;
    }
}

void GUIManager::drawSpacecraftGUI(int time) {
    if (spacecraftControls)
        spacecraftControls->draw(time);
}

void GUIManager::setupSpacecraftControls(Spacecraft* spacecraft) {
    spacecraftControls = new SpacecraftControls(engine, spacecraft);
}

void GUIManager::openSpacecraftHUD() {
    if (spacecraftControls)
        spacecraftControls->open();
}

void GUIManager::closeSpacecraftHUD() {
    if (spacecraftControls)
        spacecraftControls->close();
}

void GUIManager::setSpacecraftDistHUDSliderMin() {
    if (spacecraftControls)
        spacecraftControls->orbitDistSlider->updateMin(engine->minDistanceMod);
}

void GUIManager::setSpacecraftTimeHUDSliderMax() {
    if (spacecraftControls)
        spacecraftControls->orbitTimeSlider->updateMax(engine->maxTimeMod);
}

void GUIManager::drawBuilderGUI(int time) {
    if (builderUI)
        builderUI->draw(time);
}

Spacecraft* GUIManager::closeBuilder(KSP2D* pEngine, const Vec2D& initalPos, const Vec2D& initialVel, long double mass, Vec2D* origin) {
    Spacecraft* spacecraft = builderUI->makeShip(pEngine, initalPos, initialVel, mass, origin);
    if (spacecraft) {
        delete builderUI;
        builderUI = nullptr;
    }
    return spacecraft;
}