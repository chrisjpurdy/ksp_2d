//
// Created by Chris Purdy on 25/03/2020.
//

#include "SpacecraftControls.h"

SpacecraftControls::SpacecraftControls(KSP2D* pEngine, Spacecraft* _spacecraft) : engine(pEngine), spacecraft(_spacecraft), closeAnim(0), closeAmount(189),
                        spacecraftHUD(ImageManager::get()->getImagebyURL("spacecraftHUD.png", true, false)), objectsAdded(false) {
    thrustSlider = new GUISlider<double>(engine, 550, 750, 160, spacecraft->thrustPercent, 0, 1,
                                                                 [=](long double x){spacecraft->updateThrusters();}, 0x555500, false);
    orbitDistSlider = new GUISlider<long double>(engine, 988, 650, 160, pEngine->distModifier, pEngine->minDistanceMod, pEngine->maxDistanceMod,
                                                 [=](long double distMod){ pEngine->setPlanetScale(distMod, pEngine->orbitViewCentre); }, 0x005540, true);
    orbitTimeSlider = new GUISlider<long double>(engine, 988, 710, 160, pEngine->timeModifier, 1.0, pEngine->maxTimeMod,
                                                 [=](long double timeMod){ pEngine->speedUpGravObjs(timeMod); }, 0x550040, true);

    spacecraftHUD.setTransparencyColour(0x000000);
    prevTime = -1;
}

SpacecraftControls::~SpacecraftControls() {
}

void SpacecraftControls::update(int iCurrentTime) {

    if (prevTime == -1) {
        prevTime = iCurrentTime;
        return;
    }

    testLock();
    closeAmount += closeAnim * 0.2 * (iCurrentTime - prevTime);
    if (closeAmount <= 0) {
        closeAmount = 0;
        closeAnim = 0;
    } else if (closeAmount >= 189) {
        closeAmount = 189;
        closeAnim = 0;
    }
    prevTime = iCurrentTime;
}

void SpacecraftControls::draw(int iCurrentTime) {

    spacecraftHUD.renderImageBlit(engine, engine->getForegroundSurface(), 440, 580 + closeAmount, spacecraftHUD.getWidth()*3, spacecraftHUD.getHeight()*3, 0, 0, spacecraftHUD.getWidth(), spacecraftHUD.getHeight());

    if (closeAmount < 188) {

        auto shipVel = std::to_string((int)spacecraft->relativeVelocity.magnitude());

        engine->drawForegroundString(460, 740 + closeAmount, "Thrust", 0xffffff, engine->getFont("kenpixel.ttf", 16));
        thrustSlider->yStatic = 750 + closeAmount;
        thrustSlider->draw();

        orbitDistSlider->yStatic = 650 + closeAmount;
        orbitDistSlider->draw();
        orbitTimeSlider->yStatic = 710 + closeAmount;
        orbitTimeSlider->draw();

        engine->drawForegroundString(631 - shipVel.size() * 5.5, 704 + closeAmount, (shipVel + "m/s").c_str(), 0xffffff,
                                     engine->getFont("kenpixel.ttf", 16));
        engine->drawForegroundEllipse(650, 674 + closeAmount, 24, 24, 0xffffff);

        std::string relativeMovementTo = engine->closeByBody ? engine->closeByBody->name : "Sol";
        engine->drawForegroundString(750, 740 + closeAmount, ("Relative to: " + relativeMovementTo).c_str(), 0xffffff, engine->getFont("kenpixel.ttf", 16));

        std::string altitude = spacecraft->shipToSurfaceDist > -1 ? std::to_string(spacecraft->shipToSurfaceDist/1000) : "N/A";
        engine->drawForegroundString(750, 760 + closeAmount, ("Altitude: " + altitude + "km").c_str(), 0xffffff, engine->getFont("kenpixel.ttf", 16));


        if (engine->state == KSP2D::State::stateLowOrbit) {
            engine->drawForegroundArrow(650, 674 + closeAmount, spacecraft->screenOrientOffsetMatrix * spacecraft->relativeVelocity, 30, 0xffffff);
            engine->drawForegroundArrow(650, 674 + closeAmount, spacecraft->screenOrientMatrix * Vec2D(0, -1), 30,
                                        0xdd0000);
        } else {
            engine->drawForegroundArrow(650, 674 + closeAmount, spacecraft->relativeVelocity, 30, 0xffffff);
            engine->drawForegroundArrow(650, 674 + closeAmount, spacecraft->body->orientMatrix * Vec2D(0, -1), 30,
                                        0xdd0000);
        }

        engine->drawForegroundString(612, 623 + closeAmount, "Heading", 0xffffff, engine->getFont("kenpixel.ttf", 16));
    }
}

void SpacecraftControls::addObjectsToEngine() {
    if (objectsAdded) return;
    engine->appendObjectToArray(thrustSlider);
    thrustSlider->updateValue();
    engine->appendObjectToArray(orbitDistSlider);
    orbitDistSlider->updateValue();
    engine->appendObjectToArray(orbitTimeSlider);
    orbitTimeSlider->updateValue();
    objectsAdded = true;
}

void SpacecraftControls::testLock() {
    if (engine->timeModifier > 4) {
        thrustSlider->value = 0;
        thrustSlider->lock();
    } else {
        thrustSlider->unlock();
    }
}

void SpacecraftControls::open() {
    closeAnim = -1;
}

void SpacecraftControls::close() {
    closeAnim = 1;
}
