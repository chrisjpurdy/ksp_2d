//
// Created by Chris Purdy on 20/04/2020.
//

#ifndef KSP_2D_CW4_BUILDERUI_H
#define KSP_2D_CW4_BUILDERUI_H

#include "../KSP2D.h"
#include "GUITextbox.h"
#include "PartCatalogue.h"
#include "GUIButton.h"

class BuilderUI {
public:
    explicit BuilderUI(KSP2D* pEngine);

    ~BuilderUI();

    void draw(int iCurrentTime);

    void addObjectsToEngine();
    bool objectsAdded;

    void update(int iCurrentTime);

    KSP2D* engine;
    PartCatalogue* partCatalogue;
    GUITextbox* shipNameTextbox;
    GUIButton* makeShipButton;
    GUIButton* loadShipButton;
    GUIButton* saveShipButton;

    void setVisible(bool visible);

    Spacecraft *makeShip(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, long double mass, Vec2D *origin);
};


#endif //KSP_2D_CW4_BUILDERUI_H
