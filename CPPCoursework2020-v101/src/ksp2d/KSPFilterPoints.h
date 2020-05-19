//
// Created by Chris Purdy on 20/03/2020.
//

#ifndef KSP_2D_CW4_KSPFILTERPOINTS
#define KSP_2D_CW4_KSPFILTERPOINTS

#include "../sdl2_framework/FilterPoints.h"
#include "../sdl2_framework/SimpleTrig.h"
#include "PhysUtils/FastVec2D.h"

class FilterPointsFillIn : public FilterPoints {
public:
    explicit FilterPointsFillIn(FilterPoints* _nextFilter) : nextFilter(_nextFilter) {
    }

    FilterPointsFillIn() : nextFilter(nullptr) {
    }

    bool filter(DrawingSurface* surface, int& xVirtual, int& yVirtual, unsigned int& uiColour) override {
        transform(surface, xVirtual, yVirtual, uiColour);
        return false;
    }

    FilterPoints* nextFilter;

    void transform(DrawingSurface* surface, int& xVirtual, int& yVirtual, unsigned int& uiColour) {

        if ((nextFilter == nullptr) || nextFilter->filter(surface, xVirtual, yVirtual, uiColour)) {
            surface->rawSetPixel(xVirtual, yVirtual, uiColour);
            surface->rawSetPixel(xVirtual + 1, yVirtual + 1, uiColour);
            surface->rawSetPixel(xVirtual + 1, yVirtual - 1, uiColour);
            surface->rawSetPixel(xVirtual - 1, yVirtual + 1, uiColour);
            surface->rawSetPixel(xVirtual - 1, yVirtual - 1, uiColour);
        }
    }

};

class FilterPointsRotate : public FilterPoints {
public:
    FilterPointsRotate(Mat22* _rotMatrix, Vec2D* center, FilterPoints* _nextFilter) : rotMatrix(_rotMatrix), centerVec(*center), nextFilter(_nextFilter) {
        centerRef = center;
    }

    bool filter(DrawingSurface* surface, int& xVirtual, int& yVirtual, unsigned int& uiColour) override {
        transform(surface, xVirtual, yVirtual, uiColour);
        return false;
    }

    inline void setCenter(Vec2D* center) {
        centerRef = center;
    }

    Mat22* rotMatrix; // in radians
    Vec2D* centerRef;
    Vec2D centerVec;
    FilterPoints* nextFilter;

    void transform(DrawingSurface* surface, int& xVirtual, int& yVirtual, unsigned int& uiColour) {

        centerVec = *centerRef;

        // do the rotation about the center point
        auto pixelVec = Vec2D(xVirtual, yVirtual) - centerVec;
        pixelVec = (*rotMatrix) * pixelVec;
        pixelVec += centerVec;

        int x = pixelVec.x;
        int y = pixelVec.y;

        if ((nextFilter == nullptr) || nextFilter->filter(surface, x, y, uiColour)) {
            surface->rawSetPixel(x, y, uiColour);
        }
    }

    void bindRotMatrix(Mat22 *pMat) {
        rotMatrix = pMat;
    }
};

class FilterPointsZoom : public FilterPoints {
public:

    FilterPointsZoom(double* _zoomFactor, Vec2D* center, FilterPoints* _nextFilter) : zoomFactor(_zoomFactor), centerVec(*center), nextFilter(_nextFilter) {
        centerRef = center;
    }

    Vec2D* centerRef;
    FastVec2D centerVec;
    FilterPoints* nextFilter;
    double* zoomFactor;

    inline void setCenter(Vec2D* center) {
        centerRef = center;
    }

    bool filter(DrawingSurface* surface, int& xVirtual, int& yVirtual, unsigned int& uiColour) override {

        centerVec = *centerRef;

        int x, y;
        if (*zoomFactor != 1) {
            auto pixelVec = FastVec2D(xVirtual, yVirtual) - centerVec;
            pixelVec *= *zoomFactor;
            pixelVec += centerVec;

            x = pixelVec.x;
            y = pixelVec.y;
        } else {
            x = xVirtual; y = yVirtual;
        }


        if (*zoomFactor > 1) {
            int zX, zY, iMax = ((int) (*zoomFactor + 2.2) - 1);
            for (int ix = 0; ix < iMax; ++ix) {
                for (int iy = 0; iy < iMax; ++iy) {
                    zX = x + ix;
                    zY = y + iy;
                    if ((nextFilter == nullptr) || nextFilter->filter(surface, zX, zY, uiColour)) {
                        surface->rawSetPixel(zX, zY, uiColour);
                    }
                }
            }
        } else {
            if ((nextFilter == nullptr) || nextFilter->filter(surface, x, y, uiColour)) {
                surface->rawSetPixel(x, y, uiColour);
            }
        }

        return false;
    }

    void bindZoom(double *pDouble) {
        zoomFactor = pDouble;
    }
};

#endif //KSP_2D_CW4_KSPFILTERPOINTS
