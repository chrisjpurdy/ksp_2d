//
// Created by Chris Purdy on 18/03/2020.
//

#ifndef KSP_2D_CW4_GUIMANAGER_H
#define KSP_2D_CW4_GUIMANAGER_H

#include "../../sdl2_framework/header.h"
#include "../../sdl2_framework/BaseEngine.h"
#include "GUISlider.h"
#include "SpacecraftControls.h"
#include "../KSP2D.h"

class BuilderUI;

class GUIManager {
public:

    static void init(KSP2D *pEngine) {
        if (!singleton)
            singleton = new GUIManager(pEngine);
    }

    void addObjectsToEngine();

    static GUIManager* get() {
        return singleton;
    }

    static void uninit() {
        delete singleton;
    }

    void draw(int time, KSP2D::State state);
    void update(int time);

    void fadeTextAlert(const char* text);
    void titleFadeTextAlert(const char *text);
    void showFadePlanetLabels();

    void setupSpacecraftControls(Spacecraft* spacecraft);
    void openSpacecraftHUD();
    void closeSpacecraftHUD();
    void setSpacecraftDistHUDSliderMin();
    void setSpacecraftTimeHUDSliderMax();

    Spacecraft *closeBuilder(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, Vec2D *origin);

    void spacecraftDestroyed();

private:
    explicit GUIManager(KSP2D* pEngine);
    ~GUIManager();
    static GUIManager* singleton;
    KSP2D* engine;

    int fadeTextTimer;
    int fadeTextMaxTimer;
    std::string fadeText;

    int titleFadeTextTimer;
    int titleFadeTextMaxTimer;
    std::string titleFadeText;

    int planetLabelsTimer;
    int planetLabelsMaxTimer;

    SpacecraftControls *spacecraftControls;
    BuilderUI *builderUI;

    void drawFadeText();
    void drawOrbitGUI();
    void drawSpacecraftGUI(int time);
    void drawBuilderGUI(int time);

    void drawTitleFadeText();

};

#endif //KSP_2D_CW4_GUIMANAGER_H
