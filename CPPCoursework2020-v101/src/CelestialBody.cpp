//
// Created by Chris Purdy on 22/03/2020.
//

#include "CelestialBody.h"
#include "KSP2D.h"

CelestialBody::CelestialBody(BaseEngine* pEngine, long double x, long double y, const char* _name, const char* imgPath, const char* groundImgPath, long double radius, bool _hasAtmosphere, Vec2D* origin)
        : DisplayableObject(pEngine), PhysObject(origin), name(_name), drawState(DrawState::farOff), isCloseByBody(false), screenDistShipToSurface(10000), hasAtmosphere(_hasAtmosphere) {
    image = ImageManager::get()->getImagebyURL(imgPath, true, false);
    image.setTransparencyColour(0x000000);
    groundImage = ImageManager::get()->getImagebyURL(groundImgPath, true, false);
    shape = new Circle(radius, Vec2D(x, y));
    circumference = radius * M_PI * M_PI;
}

void CelestialBody::virtDraw() {

    if (!isVisible()) return;

    long double screenRadius = reinterpret_cast<Circle*>(shape)->radius / distMult;

    setSize(screenRadius * 2, screenRadius * 2); // should make it so this is only updated while zoom is changing
    setPosition(screenPosition.x, screenPosition.y);

    /* check if the planet is on the screen, if not return */
    if (!isOnScreen(screenPosition, screenRadius)) return;

    if (drawState == farOff) {
        /* big enough to show the proper planet */
        if (screenRadius > 1) {
            image.renderImageBlit(getEngine(), getEngine()->getForegroundSurface(),
                    screenPosition.x - screenRadius, screenPosition.y - screenRadius,
                    screenRadius * 2, screenRadius * 2, 0, 0,
                    image.getWidth(), image.getHeight());
        }
        /* not big enough to show the proper planet - show a marker instead, maybe with text */
        else {
            getEngine()->getForegroundSurface()->drawOval(screenPosition.x - 4, screenPosition.y - 4,
                                                          screenPosition.x + 4, screenPosition.y + 4,
                                                          0xffffff);
        }
    } else if (drawState == closeBy) {

        // if this is negative or 0, the whole planet fits on the screen width-wise
        long double widthOvershootPercentage = std::max((long double)0, 1 - (screenDimensions.x/(2*screenRadius)));
        long double shipSurfaceAngle = reinterpret_cast<KSP2D*>(getEngine())->closeByRotation;

        // do another check to stop drawing if surface of the planet is off the screen
        if (screenCenter.y + screenDistShipToSurface > screenDimensions.y) return;

        // draw a different view of the planets surface depending on how big it is on screen
        if (widthOvershootPercentage > 0.95) {

            long double startXPos = -abs(fmod((shipSurfaceAngle * reinterpret_cast<Circle*>(shape)->radius), screenDimensions.x * distMult));

            // first draw underground layer
            long double screenCoveredY = screenCenter.y + screenDistShipToSurface + (85 / distMult);
            long double screenCoveredX = startXPos / distMult;
            long double undergroundHeight = 19 / (groundImage.getWidth() / screenDimensions.x);
            while (screenCoveredY < screenDimensions.y) {
                groundImage.renderImageBlit(getEngine(), getEngine()->getForegroundSurface(),
                                            screenCoveredX, screenCoveredY,
                                            screenDimensions.x, undergroundHeight + 1, 0,
                                            31,
                                            groundImage.getWidth(), 19);
                groundImage.renderImageBlit(getEngine(), getEngine()->getForegroundSurface(),
                                            screenCoveredX + screenDimensions.x, screenCoveredY,
                                            screenDimensions.x, undergroundHeight + 1, 0,
                                            31,
                                            groundImage.getWidth(), 19);
                screenCoveredY += undergroundHeight;
            }

            // then draw top layer with repeated grass patterns
            while (screenCoveredX < screenDimensions.x) {
                groundImage.renderImageBlit(getEngine(), getEngine()->getForegroundSurface(),
                                            screenCoveredX, screenCenter.y + screenDistShipToSurface,
                                            screenDimensions.x / distMult + 1, 100 / distMult, 0, 0,
                                            groundImage.getWidth(), groundImage.getHeight() / 2);
                screenCoveredX += groundImage.getWidth() / distMult;
            }
        } else {
            // this is very memory expensive, but a bit less so thanks to the sdl2 gfx rotozoom function
            // renders the planet rotated on screen
            image.renderImageBlitRoatated(getEngine(), getEngine()->getForegroundSurface(), shipSurfaceAngle * radToDeg,
                                  0, screenCenter.y + screenDistShipToSurface,
                                  screenDimensions.x, screenDimensions.y/2.0,image.getWidth()*(widthOvershootPercentage/2), 0,
                                  image.getWidth()*(1-widthOvershootPercentage), image.getHeight()*(1-widthOvershootPercentage)/4.0);
        }
    } else if (drawState == none) {
        // dont draw
    }
}

void CelestialBody::virtDoUpdate(int iCurrentTime) {
    tick(iCurrentTime);
    redrawDisplay();
}

bool CelestialBody::isOnScreen(Vec2D& screenPos, long double screenRadius) {
    return (screenPos - screenCenter).magnitudeSquared() < (screenViewRadius + screenRadius) * (screenViewRadius + screenRadius);
}

void CelestialBody::setCloseByBody(bool b) {
    isCloseByBody = b;
    gravityAtSurface = PhysBody::G * (body->mass_data.mass / (reinterpret_cast<Circle*>(shape)->radius * reinterpret_cast<Circle*>(shape)->radius));
    body->closeByBody = b;
}

// takes an angle from 0 -> 2PI and returns the position on the surface of the body's surface corresponding to
// that rotation clockwise form the north pole
Vec2D CelestialBody::getPositionOnSurface(double radians, long double heightAbove) {
    Vec2D radiusLine(0, -(reinterpret_cast<Circle*>(shape)->radius + heightAbove));
    radiusLine.rotate(radians);
    return body->position + radiusLine;
}

CelestialBody::~CelestialBody() {
    getEngine()->removeDisplayableObject(this);
}
