//
// Created by Chris Purdy on 08/03/2020.
//

#include "KSP2D.h"
#include "Spacecraft/BasicSpacecraft.h"
#include "GUI/GUIManager.h"

SimpleTrig* SimpleTrig::singleton = nullptr;

void KSP2D::virtSetupBackgroundBuffer() {
    fillBackground(0x02020d);
}

int KSP2D::virtInitialise() {
    isKeyListener = true;
    bgCleared = false;
    notifyObjectsAboutMouse(true);
    notifyObjectsAboutKeys(true);
    factoryBg = ImageManager::get()->loadImage("factory.png");
    return BaseEngine::virtInitialise();
}

/**
 * This sets up all of the planets and the planetary objects (trees, buildings, rocks etc)
 *
 * @return
 */
int KSP2D::virtInitialiseObjects() {

    destroyOldObjects(true);
    createObjectArray(0);

    planets.push_back(new Sun(this, "Sol", "sun.png", "sunground.png", 696340000, 1989000000000000000000000000000.0, 0.0, 0, orbitViewCentre));
    planets.push_back(new Planet(this, "Kerbine", "kerbin.png", "ground.png", 6371000, 5972000000000000000000000.0, 148890000000.0, 0, orbitViewCentre, 0.0167, 31600000.0, 0.4, planets[0]));
    planets.push_back(new Planet(this, "The Mune", "mune.png", "moonground.png", 1737000, 73476730000000000000000.0, 149270000000.0 - 148890000000.0, 0, orbitViewCentre, 0.0549, 2400000.0, 0.5, planets[1]));
    planets.push_back(new Planet(this, "Mercurio", "9.png", "moonground.png", 4880000, 330110000000000000000000.0, 57909050000.0, 0, orbitViewCentre, 0.20563, 7600000.0, 0.3, planets[0]));
    planets.push_back(new Planet(this, "Veenous", "19.png", "moonground.png", 6051800, 4867500000000000000000000.0, 108208000000.0, 0, orbitViewCentre, 0.006772, 19400000.0, 1.4, planets[0]));
    planets.push_back(new Planet(this, "Mayers", "14.png", "ground.png", 3389500, 604171000000000000000000.0, 227939200000.0, 0, orbitViewCentre, 0.0934, 59400000.0, 0.8, planets[0]));
    planets.push_back(new Planet(this, "Haley's Comet", "mune.png", "moonground.png", 5500, 220000000000000.0, 2667928430000.0, 0, orbitViewCentre, 0.96714, 2400000000.0, 2.1, planets[0]));

    // TODO when the Moon is the view center and time is sped up, drawing of orbits seems to break occasionally
    for (auto* p : planets) {
        p->body->updatePosition(1);
        p->recalcShape();
        appendObjectToArray(p);
        p->recalcScreenPos();
    }

    gravObjViewCenter = 1;
    orbitViewCentre = &(planets.at(gravObjViewCenter)->body->position);
    minDistanceMod = reinterpret_cast<Circle*>(planets.at(gravObjViewCenter)->shape)->radius / 400.0;
    GUIManager::get()->setSpacecraftDistHUDSliderMin();
    distModifier = reinterpret_cast<Circle*>(planets.at(gravObjViewCenter)->shape)->radius / 250.0;
    GUIManager::get()->fadeTextAlert(planets.at(gravObjViewCenter)->name.c_str());
    setPlanetScale(distModifier, orbitViewCentre);

    planetDecorations.push_back(new PlanetObject(this, planets.at(1), 1.56842, "building_0", 1, 1, 3));
    planetDecorations.push_back(new PlanetObject(this, planets.at(1), 1.56830, "building_1", 1, 1, 3));
    planetDecorations.push_back(new PlanetObject(this, planets.at(1), 1.56835, "building_2", 1, 1, 3));
    planetDecorations.push_back(new PlanetObject(this, planets.at(1), 1.56836, "tree_1", 2.5, 2.5, 14));

    for (auto * pd : planetDecorations) {
        appendObjectToArray(pd);
    }

    GUIManager::get()->addObjectsToEngine();

    timeModifier = 1;
    GUIManager::get()->fadeTextAlert("Press b to create a spacecraft");

    return 0;
}

void KSP2D::buildPlayerSpacecraft() {

    state = stateBuilder;
    //fillBackground(0x888880);
    factoryBg.renderImage(getBackgroundSurface(),0,0,0,0,1300,800);

    for (auto planet : planets) {
        planet->setVisible(false);
    }
    for (auto pd : planetDecorations) {
        pd->setVisible(false);
    }


}

void KSP2D::addNewSpacecraft() {

    if (playerSpacecraft) return;

    playerSpacecraft = GUIManager::get()->closeBuilder(this, planets[1]->getPositionOnSurface(1.56835, 30),
            planets[1]->body->velocity, orbitViewCentre);
    if (!playerSpacecraft) {
        GUIManager::get()->fadeTextAlert("Please create a valid craft");
        return;
    }
    spacecraft.push_back(playerSpacecraft);
    //delete playerSpacecraft; // TODO just testing that it deletes properly currently

    state = stateLowOrbit;

    fillBackground(0x02020d);

    for (auto planet : planets) {
        planet->setVisible(true);
    }
    for (auto pd : planetDecorations) {
        pd->setVisible(true);
    }

//    playerSpacecraft = new BasicSpacecraft(this, "basicRocket_0",
//                                           planets[1]->getPositionOnSurface(1.56835, 30),
//                                           planets[1]->body->velocity, 10000, orbitViewCentre);
    //spacecraft.push_back(playerSpacecraft);

    //playerSpacecraft->body->accuTimeSlice = planets[1]->body->accuTimeSlice;
    //TODO ground moves away immediately by a certain amount (different each time)? PATCHED BUT AWFULLY SEE REPOSSPACECRAFT
    reposSpacecraft = true;

    m_vecDisplayableObjects.insert(m_vecDisplayableObjects.begin(), playerSpacecraft);
    GUIManager::get()->setupSpacecraftControls(playerSpacecraft);
    GUIManager::get()->addObjectsToEngine();

    gravObjViewCenter = -1;
    orbitViewCentre = &playerSpacecraft->body->position;
    minDistanceMod = 1;
    GUIManager::get()->setSpacecraftDistHUDSliderMin();
    distModifier = 1;
    GUIManager::get()->fadeTextAlert(("Spacecraft: " + playerSpacecraft->shipName).c_str());
    setPlanetScale(distModifier, orbitViewCentre);

    playerSpacecraft->thrustPercent = 0;
    timeModifier = 1;
}

KSP2D::KSP2D() : state(stateOrbit), freeMovingOrbitViewCentre(Vec2D(0,0)), orbitViewCentre(&freeMovingOrbitViewCentre), reposSpacecraft(false) {
    gravObjViewCenter = -1; // index of grav object being used as view center (-1 if its centered on the player)
    changeOriginLock = false;
    GUIManager::init(this);
    SimpleTrig::init();
}

// stuff to do preupdate, whether or whether not update is actually called
void KSP2D::virtMainLoopPreUpdate() {
    //std::cout << "earth moon dist: " << ((planets.at(1)->gravObj.position) - (planets.at(2)->gravObj.position)).magnitude() << std::endl;
    checkKeyInputs();
    // calculate sum of all forces acting on each object
    // maybe only do this every few frames or something to reduce computation load
    for (auto* s : spacecraft) {
        for (auto* p : planets) {
            s->body->applyGravity(p->body);
            s->applyGadgets();
        }
        s->body->recalcForce = false;
    }

    if (playerSpacecraft) {
        for (auto* p : planets) {
            if (playerSpacecraft->checkProximityChange(p)) break;
        }
    }

    GUIManager::get()->update(m_iTick);
}

void KSP2D::speedUpGravObjs(long double timeMod) {
    for (auto* p : planets) {
        p->setTimeModifier(timeMod);
    }
    for (auto* s : spacecraft) {
        s->setTimeModifier(timeMod);
    }
}

void KSP2D::setPlanetScale(long double distMod, Vec2D* origin) {
    for (auto* p : planets) {
        p->setDistModifier(distMod);
        p->setScreenOrigin(origin);
    }
    for (auto* s : spacecraft) {
        s->setDistModifier(distMod);
        s->setScreenOrigin(origin);
        s->isChanged = true;
    }
    bgCleared = false;
}

void KSP2D::virtMouseDown(int iButton, int x, int y) {
    isKeyListener = true;
}

void KSP2D::checkKeyInputs() {

    if (!isKeyListener) return;

    if (state != stateBuilder) {
        if (isKeyPressed(SDLK_z)) {
            if (!changeOriginLock) {
                gravObjViewCenter = (planets.size() + (gravObjViewCenter - 1)) % (int) planets.size();
                orbitViewCentre = &planets.at(gravObjViewCenter)->body->position;
                minDistanceMod = reinterpret_cast<Circle *>(planets.at(gravObjViewCenter)->shape)->radius / 400.0;
                GUIManager::get()->setSpacecraftDistHUDSliderMin();
                distModifier = reinterpret_cast<Circle *>(planets.at(gravObjViewCenter)->shape)->radius / 250.0;
                GUIManager::get()->fadeTextAlert(planets.at(gravObjViewCenter)->name.c_str());
                setPlanetScale(distModifier, orbitViewCentre);
                changeOriginLock = true;
            }
        } else if (isKeyPressed(SDLK_x)) {
            if (!changeOriginLock) {
                gravObjViewCenter = (gravObjViewCenter + 1) % (int) planets.size();
                orbitViewCentre = &planets.at(gravObjViewCenter)->body->position;
                minDistanceMod = reinterpret_cast<Circle *>(planets.at(gravObjViewCenter)->shape)->radius / 400.0;
                GUIManager::get()->setSpacecraftDistHUDSliderMin();
                distModifier = reinterpret_cast<Circle *>(planets.at(gravObjViewCenter)->shape)->radius / 250.0;
                GUIManager::get()->fadeTextAlert(planets.at(gravObjViewCenter)->name.c_str());
                setPlanetScale(distModifier, orbitViewCentre);
                changeOriginLock = true;
            }
        } else if (isKeyPressed(SDLK_s)) {
            if (!changeOriginLock && playerSpacecraft) {
                gravObjViewCenter = -1;
                orbitViewCentre = &playerSpacecraft->body->position;
                minDistanceMod = 1;
                GUIManager::get()->setSpacecraftDistHUDSliderMin();
                distModifier = 1.3;
                GUIManager::get()->fadeTextAlert(("Spacecraft: " + playerSpacecraft->shipName).c_str());
                setPlanetScale(distModifier, orbitViewCentre);
                changeOriginLock = true;
            }
        } else {
            changeOriginLock = false;
        }

        if (playerSpacecraft && timeModifier < 4) {
            if (isKeyPressed(SDLK_a)) {
                playerSpacecraft->rotate(1);
            } else if (isKeyPressed(SDLK_d)) {
                playerSpacecraft->rotate(-1);
            } else {
                playerSpacecraft->rotate(0);
            }
        }

        if (isKeyPressed(SDLK_UP)) {
            GUIManager::get()->openSpacecraftHUD();
        } else if (isKeyPressed(SDLK_DOWN)) {
            GUIManager::get()->closeSpacecraftHUD();
        }

        if (isKeyPressed(SDLK_l))
            GUIManager::get()->showFadePlanetLabels();

        if (isKeyPressed(SDLK_b)) {
            if (!playerSpacecraft) {
                buildPlayerSpacecraft();
                m_pKeyStatus[SDLK_b] = false;
            }
        }
    }

}

void KSP2D::virtPostDraw() {
    for (auto* s : spacecraft) {
        if (s != playerSpacecraft)
            m_pForegroundSurface->copyRectangleFrom(&s->surface, 0, 0, 1300, 800);
    }

    if (playerSpacecraft) m_pForegroundSurface->copyRectangleFrom(&playerSpacecraft->surface, 0, 0, 1300, 800);
    GUIManager::get()->draw(getRawTime(), state);

    if (!bgCleared && gravObjViewCenter != -1) {
        fillBackground(0x02020d);
        bgCleared = true;
    }
}

void KSP2D::virtCleanUp() {
    GUIManager::uninit();
}

void KSP2D::drawForegroundEllipse(long int centerX, long int centerY, long int radiusX, long int radiusY, unsigned int uiColour) {
    long int x, y, xChange, yChange, ellipseError, twoASquare, twoBSquare, stoppingX, stoppingY;

    twoASquare = 2*radiusX*radiusX;
    twoBSquare = 2*radiusY*radiusY;
    x = radiusX;
    y = 0;
    xChange = radiusY*radiusY*(1 - 2*radiusX);
    yChange = radiusX*radiusX;
    ellipseError = 0;
    stoppingX = twoBSquare*radiusX;
    stoppingY = 0;

    while (stoppingX >= stoppingY) {
        plot4EllipsePoints(centerX, centerY, x, y, uiColour);
        ++y;
        stoppingY += twoASquare;
        ellipseError += yChange;
        yChange += twoASquare;
        if ((2*ellipseError + xChange) > 0) {
            --x;
            stoppingX -= twoBSquare;
            ellipseError += xChange;
            xChange += twoBSquare;
        }
        ++y;
        stoppingY += twoASquare;
        ellipseError += yChange;
        yChange += twoASquare;
        if ((2*ellipseError + xChange) > 0) {
            --x;
            stoppingX -= twoBSquare;
            ellipseError += xChange;
            xChange += twoBSquare;
        }
    }

    x = 0;
    y = radiusY;
    xChange = radiusY*radiusY;
    yChange = radiusX*radiusX*(1 - 2*radiusY);
    ellipseError = 0;
    stoppingX = 0;
    stoppingY = twoASquare*radiusY;
    while (stoppingX <= stoppingY) {
        plot4EllipsePoints(centerX, centerY, x, y, uiColour);
        ++x;
        stoppingX += twoBSquare;
        ellipseError += xChange;
        xChange += twoBSquare;
        if ((2*ellipseError + yChange) > 0) {
            --y;
            stoppingY -= twoASquare;
            ellipseError += yChange;
            yChange += twoASquare;
        }
    }

}

void KSP2D::plot4EllipsePoints(long int centerX, long int centerY, long int x, long int y, unsigned int uiColour) {
    getForegroundSurface()->setPixel(centerX + x, centerY + y, uiColour);
    getForegroundSurface()->setPixel(centerX - x, centerY + y, uiColour);
    getForegroundSurface()->setPixel(centerX + x, centerY - y, uiColour);
    getForegroundSurface()->setPixel(centerX - x, centerY - y, uiColour);
}

void KSP2D::drawForegroundArrow(int x, int y, Vec2D vec, int length, unsigned int uiColour) {
    vec.normalise();
    double xLen = (vec.x * length)/2.0;
    double yLen = (vec.y * length)/2.0;
    drawForegroundLine(x - xLen, y - yLen, x + xLen, y + yLen, uiColour);
    drawForegroundRectangle(x + xLen - 2, y + yLen - 2, x + xLen + 2, y + yLen + 2, uiColour);
}

void KSP2D::virtMainLoopDoBeforeUpdate() {
}

void KSP2D::virtMainLoopPostUpdate() {
    if (reposSpacecraft) {
        // currently this is used to reposition a new spacecraft on the surface of Kerbine
        playerSpacecraft->body->position = planets[1]->getPositionOnSurface(1.56835, 30);
        playerSpacecraft->body->rotate(-1.56835);
        reposSpacecraft = false;
    }
}

void KSP2D::changeSkyColour(double x) {
    double mixPercentage = pow(x,4);
    uint32_t r = 135 * mixPercentage, g = 206 * mixPercentage, b = 235 * mixPercentage;
    uint32_t colour = b + (g << 8u) + (r << 16u);
    fillBackground((colour - 0x02020d) + 0x02020d);
    bgCleared = false;
}
