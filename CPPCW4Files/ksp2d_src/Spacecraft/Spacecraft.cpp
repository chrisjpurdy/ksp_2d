//
// Created by Chris Purdy on 09/03/2020.
//

#include "Spacecraft.h"

#include <utility>
#include "../GUI/GUIManager.h"

void Spacecraft::virtDraw() {

    if (!isVisible()) return;

//    long double screenWidth = data->width / distMult;
    long double screenWidth = 300.0 / distMult;
//    long double screenHeight = data->height / distMult;;
    long double screenHeight = 300.0 / distMult;
    setSize(screenWidth, screenHeight);
    setPosition(screenPosition.x, screenPosition.y);

//    zoomAmount = screenWidth / data->width;
    zoomAmount = screenWidth / 300.0;

    if (thrustPercent > 0) {
        isChanged = true;
        thrustSwitchedOff = false;
    } else if (thrustPercent <= 0 && !thrustSwitchedOff) {
        isChanged = true;
        thrustSwitchedOff = true;
    }

    if (!isOnScreen(screenPosition, screenWidth, screenHeight)) {
        surface.fillSurface(0x000000);
        surface.setAlpha(0);
        return;
    }

    if (isChanged) {
        surface.fillSurface(0x000000);
        surface.setAlpha(0);
    }

    /* big enough to show */
    if (screenWidth > 2 && screenHeight > 2) {
        if(!isChanged) return; // don't draw if the spacecraft hasn't been modified (scaled or rotated)

        if (thrustPercent > 0) {
            thrustSprite.draw(getEngine(), &surface, screenPosition.x - thrustSprite.spriteSheet.getWidth()/10.0, screenPosition.y + (300.0/2.0) - 15);
        }

        surface.copyRectangleFrom(&partsSurface,0,0,partsSurface.getSurfaceWidth(),partsSurface.getSurfaceHeight());
    }
        /* not big enough to show - show a marker instead */
    else {
        getKSPEngine()->getForegroundSurface()->drawOval(screenPosition.x - 4, screenPosition.y - 4,
                                                         screenPosition.x + 4, screenPosition.y + 4,
                                                         0xee0f0f);
    }

    isChanged = false;

}

Spacecraft::Spacecraft(KSP2D* pEngine, const Vec2D& initalPos, const Vec2D& initialVel, long double mass, Vec2D* origin, std::vector<RocketPart*>& _parts)
        : DisplayableObject(pEngine), PhysObject(origin), surface(pEngine), partsSurface(pEngine),
          spacecraftZoomFilter(nullptr, &screenPosition, &spacecraftBoundsFilter),
          spacecraftRotateFilter(nullptr, &screenPosition, &spacecraftZoomFilter), thrustSprite("flame.png", 5, 5) {

    for (auto* part : _parts) parts.push_back(part);
    body = new PhysBody(this, mass, initalPos, initialVel);
    body->orient = 0;
    body->orientMatrix = Mat22(body->orient);
    screenOrient = 0;
    screenOrientOffset = 0;
    body->mass_data.inertia = 0.5;
    body->mass_data.inverse_inertia = 1.0/body->mass_data.inertia;
    // TODO setup the ships physics object so that it is properly to scale with the system (is currently 300m tall or something stupid)
//    Vec2D tl(initalPos.x - data->width/4.0, initalPos.y - data->height/2.0);
    Vec2D tl(initalPos.x - 300.0/4.0, initalPos.y - 300.0/2.0);
//    Vec2D bl(initalPos.x - data->width/4.0, initalPos.y + data->height/2.0);
    Vec2D bl(initalPos.x - 300.0/4.0, initalPos.y + 300.0/2.0);
    shape = new OBB(tl, bl, body->position, body->orientMatrix);

    zoomAmount = 0;
    isChanged = true;
    thrustSwitchedOff = true;
    thrustPercent = 0;
    thrustStrength = 500000; // whats a good value??? - 12MN at sea level for a massive rocket apparently, 2950 tones for saturn 5

    surface.createSurface(screenDimensions.x, screenDimensions.y);
    surface.addBoundsCheck(0, screenDimensions.x,0, screenDimensions.y);
    surface.fillSurface(0x000000);
    surface.setAlpha(0);
    // background filter chains onto the rotation filter
    surface.setDrawPointsFilter(&spacecraftRotateFilter);

    partsSurface.createSurface(screenDimensions.x, screenDimensions.y);
    partsSurface.addBoundsCheck(0, screenDimensions.x,0, screenDimensions.y);

    for (auto part : parts) {
        getEngine()->removeDisplayableObject(part->sprite);
    }
    redrawParts();
    calcAeroProfile();

    spacecraftRotateFilter.bindRotMatrix(&screenOrientMatrix);
    updateScreenOrient();
    spacecraftZoomFilter.bindZoom(&zoomAmount);
}

KSP2D* Spacecraft::getKSPEngine() {
    return (KSP2D*)m_pEngine;
}

void Spacecraft::virtDoUpdate(int iCurrentTime) {
    checkSurfaceCollision(getKSPEngine()->closeByBody);
    tick(iCurrentTime);
    // TODO only redraw if craft is not the center (i.e. being controlled), hasn't rotated and doesn't need scaling
}

bool Spacecraft::isOnScreen(Vec2D& screenPos, long double screenWidth, long double screenHeight) {
    return screenPos.x - screenWidth / 2.0 < screenDimensions.x && screenPos.x + screenWidth / 2.0 > 0
           && screenPos.y - screenHeight / 2.0 < screenDimensions.y && screenPos.y + screenHeight / 2.0 > 0;
}

void Spacecraft::rotate(double angle, long double timeMod) {
    body->applyRotation(angle);
    updateScreenOrient();
    //std::cout << "Body orient: " << body->orient << ", screen orient: " << screenOrient << std::endl;
    isChanged = true;
}

void Spacecraft::applyThrust() {
    if (thrustPercent > 0)
        body->applyForceFromDir( body->orientMatrix, -thrustPercent * thrustStrength);
}

bool Spacecraft::checkProximityChange(CelestialBody* planet) {

    Vec2D posDiff = body->position - planet->body->position;
    long double xyDiff = abs(posDiff.x) + abs(posDiff.y);

    // if x diff plus y diff is more than 1.5 times the max amount I'm comparing the actual distance against, don't bother calculating
    if (xyDiff > reinterpret_cast<Circle*>(planet->shape)->radius * 3) return false;

    long double otherDistance = posDiff.magnitude();
    long double shipToSurfaceDist = otherDistance - reinterpret_cast<Circle*>(planet->shape)->radius;
    planet->screenDistShipToSurface = shipToSurfaceDist / distMult;
    if (shipToSurfaceDist / reinterpret_cast<Circle*>(planet->shape)->radius < stateChangeAltitudeRadiusMult) {
        getKSPEngine()->closeByBody = planet;
        planet->setCloseByBody(true);
        getKSPEngine()->maxTimeMod = 20;
        GUIManager::get()->setSpacecraftTimeHUDSliderMax();

        // rotates the ship on screen relative to where on the planets surface it is
        long double dot = rotationNormalAxis.dotproduct(posDiff);
        long double det = rotationNormalAxis.x * posDiff.y - posDiff.x * rotationNormalAxis.y;
        updateScreenOrient(std::atan2(det, dot));
        isChanged = true; // make sure the rotation gets updated on screen

        getKSPEngine()->closeByRotation = screenOrientOffset;
        if (planet->screenDistShipToSurface < screenViewRadius + 150) {
            if (reinterpret_cast<Circle*>(planet->shape)->radius / distMult > 500) {
                planet->drawState = CelestialBody::DrawState::closeBy;
                getKSPEngine()->state = KSP2D::stateLowOrbit;
            } else {
                planet->drawState = CelestialBody::DrawState::farOff;
                getKSPEngine()->state = KSP2D::stateOrbit;
            }
        } else {
            /* dont draw the planet since this else branch implies the planet is not on screen */
            planet->drawState = CelestialBody::DrawState::none;
            getKSPEngine()->state = KSP2D::stateLowOrbit;
        }
        return true;
    } else {
        getKSPEngine()->maxTimeMod = 100000000;
        GUIManager::get()->setSpacecraftTimeHUDSliderMax();
        planet->setCloseByBody(false);
        updateScreenOrient(0);
        isChanged = true;
        getKSPEngine()->closeByRotation = screenOrientOffset;
        getKSPEngine()->state = KSP2D::stateOrbit;
        getKSPEngine()->closeByBody = nullptr;
        planet->screenDistShipToSurface = -1;
        planet->drawState = CelestialBody::DrawState::farOff;
        return false;
    }
}

void Spacecraft::checkSurfaceCollision(CelestialBody* planet) {

    if (!planet) return;

    Vec2D posDiff = body->position - planet->body->position;
    long double xyDiff = abs(posDiff.x) + abs(posDiff.y); // Manhattan distance heuristic
    // if x diff plus y diff is more than 1.5 times the max amount I'm comparing the actual distance against, don't bother calculating
    if (xyDiff > reinterpret_cast<Circle*>(planet->shape)->radius * 3) return;

    CollisionManifold manifold(this, planet);

    // resolve collision if colliding
    if(PhysBody::OBBCircleCollision(manifold))
        PhysBody::resolveCollision(manifold);

}

void Spacecraft::checkAtmosphericForces(CelestialBody* planet) {

    if (!planet) return;

    Vec2D posDiff = body->position - planet->body->position;
    long double xyDiff = abs(posDiff.x) + abs(posDiff.y); // Manhattan distance heuristic
    if (xyDiff > reinterpret_cast<Circle*>(planet->shape)->radius * 3) return;



}

void Spacecraft::redrawParts() {
    partsSurface.fillSurface(0x000000);
    partsSurface.setAlpha(0);
    for (auto part : parts) {
        part->sprite->drawOnSurface(&partsSurface);
    }
}

void Spacecraft::calcAeroProfile() {

    int partsConsideredPerSide[4] = {0, 0, 0, 0};

    for (auto part : parts) {
        /* doesn't calculate the aerodynamics perfectly - a square of 4 normal body pieces wouldn't be properly taken into account
         * due to the way they connect between eachother */
        for (int i=0; i<4; i++) {
            /* if the part has no part connected to it on a side */
            if (!part->partData->anchorPoints[i].connectedPart) {
                /* then add that parts aerodynamics to the spaceships aerodynamics for that same side */
                partsConsideredPerSide[i]++;
                aeroProfile[i].scaleFactor += part->partData->aerodynamicProfile[i].scaleFactor;
                aeroProfile[i].rating += part->partData->aerodynamicProfile[i].rating;
            }
        }
    }
    /* now average the values for each side depending on how many parts were considered */
    //std::cout << "Aero profile: " << std::endl;
    for (int i=0; i<4; i++) {
        //std::cout << i << std::endl;
        aeroProfile[i].scaleFactor /= partsConsideredPerSide[i];
        //std::cout << aeroProfile[i].scaleFactor << std::endl;
        aeroProfile[i].rating /= partsConsideredPerSide[i];
        //std::cout << aeroProfile[i].rating << std::endl;
    }


}

Spacecraft::~Spacecraft() {
    for (auto part : parts) {
        delete part;
    }
}
