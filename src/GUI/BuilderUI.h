//
// Created by Chris Purdy on 20/04/2020.
//

#ifndef KSP_2D_CW4_BUILDERUI_H
#define KSP_2D_CW4_BUILDERUI_H

#include "../KSP2D.h"
#include "GUITextbox.h"
#include "PartCatalogue.h"
#include "GUIButton.h"
#include <sstream>
#include <fstream>

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

    SimpleImage* shipInfoPanel;

    unsigned int shipOffsetCommandNotification;

    void setVisible(bool visible);

    Spacecraft *makeShip(KSP2D *pEngine, const Vec2D &initalPos, const Vec2D &initialVel, Vec2D *origin);

    void drawShipInfo(DrawingSurface *pSurface);

    void saveShip();

    void loadShip();

    std::string stringifyPartData(PartData *data, int sideToIgnore);

    bool recursivelyAddParts(rapidjson::Value &part, RocketPart::Side side, RocketPart *parentPart,
                             SpacecraftParts *spacecraftParts, std::vector<RocketPart *>& sideMostParts);
};


#endif //KSP_2D_CW4_BUILDERUI_H
