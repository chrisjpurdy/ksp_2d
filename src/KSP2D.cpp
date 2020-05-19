//
// Created by Chris Purdy on 08/03/2020.
//

#include "KSP2D.h"
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

    planets.push_back(new Sun(this, "Sol", "sun.png", "sunground.png", 696340000, 1989000000000000000000000000000.0, 0.0, 0.0, orbitViewCentre));
    planets.push_back(new Planet(this, "Kerbine", "kerbin.png", "ground.png", 6371000, 5972000000000000000000000.0, 148890000000.0, 0, orbitViewCentre, 0.0167, 31600000.0, 0.4, planets[0]));
    planets.push_back(new Planet(this, "The Mune", "mune.png", "moonground.png", 1737000, 73476730000000000000000.0, 149270000000.0 - 148890000000.0, 0, orbitViewCentre, 0.0549, 2400000.0, 0.5, planets[1]));
    planets.push_back(new Planet(this, "Mercurio", "mercurio.png", "moonground.png", 4880000, 330110000000000000000000.0, 57909050000.0, 0, orbitViewCentre, 0.20563, 7600000.0, 0.3, planets[0]));
    planets.push_back(new Planet(this, "Veenous", "veenous.png", "moonground.png", 6051800, 4867500000000000000000000.0, 108208000000.0, 0, orbitViewCentre, 0.006772, 19400000.0, 1.4, planets[0]));
    planets.push_back(new Planet(this, "Mayers", "mayers.png", "ground.png", 3389500, 604171000000000000000000.0, 227939200000.0, 0, orbitViewCentre, 0.0934, 59400000.0, 0.8, planets[0]));
    planets.push_back(new Planet(this, "Haley's Comet", "mune.png", "moonground.png", 5500, 220000000000000.0, 2667928430000.0, 0, orbitViewCentre, 0.96714, 2400000000.0, 2.1, planets[0]));

    // TODO when the Moon is the view center and time is sped up a lot, drawing of orbits seems to go weird
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
    GUIManager::get()->titleFadeTextAlert("Gerbil Space Program");
    GUIManager::get()->fadeTextAlert("Press b to create a spacecraft");

    return 0;
}

/**
 * Called when 'b' is pressed if there is no spacecraft - allows for switching to the building GUI
 */
void KSP2D::buildPlayerSpacecraft() {

    GUIManager::get()->titleFadeTextAlert("");

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

/**
 * Is bound as a callback function to the "Create ship" button in the builder UI, creates the spacecraft and changes the game
 * to 'flight' mode
 */
void KSP2D::addNewSpacecraft() {

    if (playerSpacecraft) return;

    playerSpacecraft = GUIManager::get()->closeBuilder(this, planets[1]->getPositionOnSurface(1.56835, 30),
            planets[1]->body->velocity, orbitViewCentre);
    if (!playerSpacecraft) {
        GUIManager::get()->fadeTextAlert("Please create a valid craft");
        return;
    }
    spacecraft.push_back(playerSpacecraft);

    state = stateLowOrbit;

    fillBackground(0x02020d);

    for (auto planet : planets) {
        planet->setVisible(true);
    }
    for (auto pd : planetDecorations) {
        pd->setVisible(true);
    }

    //TODO ground moves away immediately by a certain amount (different each time)? - patched but not sufficiently imo
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

KSP2D::KSP2D() : state(stateOrbit), freeMovingOrbitViewCentre(Vec2D(-100000000,100000000)), orbitViewCentre(&freeMovingOrbitViewCentre), reposSpacecraft(false) {
    gravObjViewCenter = -1; // index of grav object being used as view center (-1 if its centered on the player)
    changeOriginLock = false;
    GUIManager::init(this);
    SimpleTrig::init();
}

/**
 * Handles application of gravity between objects, and application of gadgets to modify each ship
 */
void KSP2D::virtMainLoopPreUpdate() {

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

/**
 * Speeds up the physics objects associated with each spaceship and planet
 * Called using the time warp slider
 *
 * @param timeMod   the time modifier to speed up the movement of objects by
 */
void KSP2D::speedUpGravObjs(long double timeMod) {
    for (auto* p : planets) {
        p->setTimeModifier(timeMod);
    }
    for (auto* s : spacecraft) {
        s->setTimeModifier(timeMod);
    }
}

/**
 * Changes the zoom of planets and objects
 * Called using the zoom slider
 *
 * @param distMod   how many pixels per pixel on the screen (1 would mean real size)
 * @param origin    the centre of the zoom
 */
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
                bgCleared = false;
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
                bgCleared = false;
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

/**
 * Handles the spacecraft being destroyed, called when it hits the surface of a planet too fast
 */
void KSP2D::spacecraftDestroyed() {
    for (int i=0; i<spacecraft.size(); i++) {
        if (spacecraft[i] == playerSpacecraft) {
            spacecraft.erase(spacecraft.begin() + i);
            break;
        }
    }
    removeDisplayableObject(playerSpacecraft);
    delete playerSpacecraft;
    playerSpacecraft = nullptr;
    reposSpacecraft = false;
    state = stateOrbit;

    GUIManager::get()->spacecraftDestroyed();

    gravObjViewCenter = 1;
    orbitViewCentre = &(planets.at(gravObjViewCenter)->body->position);
    minDistanceMod = reinterpret_cast<Circle*>(planets.at(gravObjViewCenter)->shape)->radius / 400.0;
    GUIManager::get()->setSpacecraftDistHUDSliderMin();
    distModifier = reinterpret_cast<Circle*>(planets.at(gravObjViewCenter)->shape)->radius / 250.0;
    GUIManager::get()->fadeTextAlert(planets.at(gravObjViewCenter)->name.c_str());
    setPlanetScale(distModifier, orbitViewCentre);

    timeModifier = 1;
    GUIManager::get()->titleFadeTextAlert("You hit the floor hard");
    GUIManager::get()->fadeTextAlert("Press b to create a spacecraft");
}

void KSP2D::virtCleanUp() {
    GUIManager::uninit();
}

/**
 * Draws an ellipse using the mid-point method
 *
 * @param centerX
 * @param centerY
 * @param radiusX
 * @param radiusY
 * @param uiColour
 */
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

/**
 * Used by drawForegroundEllipse to draw 4 points per point calculated (since ellipses have two lines of symmetry)
 *
 * @param centerX
 * @param centerY
 * @param x
 * @param y
 * @param uiColour
 */
void KSP2D::plot4EllipsePoints(long int centerX, long int centerY, long int x, long int y, unsigned int uiColour) {
    getForegroundSurface()->setPixel(centerX + x, centerY + y, uiColour);
    getForegroundSurface()->setPixel(centerX - x, centerY + y, uiColour);
    getForegroundSurface()->setPixel(centerX + x, centerY - y, uiColour);
    getForegroundSurface()->setPixel(centerX - x, centerY - y, uiColour);
}

/**
 * Draws an arrow, used to show velocity and heading in the spacecraft control panel
 *
 * @param x         x centre of the arrow
 * @param y         y centre of the arrow
 * @param vec       the heading of the arrow
 * @param length    the display length in pixels
 * @param uiColour
 */
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
        //this is used to reposition a new spacecraft on the surface of Kerbine
        playerSpacecraft->body->position = planets[1]->getPositionOnSurface(1.56835, 30);
        playerSpacecraft->body->rotate(-1.56835);
        reposSpacecraft = false;
    }
}

/**
 * Mixes the 'space' background colour and a blue sky colour together depending on how close the ship is to a planet
 *
 * @param zoomPlanetDist     0 - 1, a measure of both how low the ship on a planet is and how zoomed in the camera is
 */
void KSP2D::changeSkyColour(double zoomPlanetDist) {
    double mixPercentage = pow(zoomPlanetDist,4);
    uint32_t r = 135 * mixPercentage, g = 206 * mixPercentage, b = 235 * mixPercentage;
    uint32_t colour = b + (g << 8u) + (r << 16u);
    fillBackground((colour - 0x02020d) + 0x02020d);
    bgCleared = false;
}
