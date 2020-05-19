//
// Created by Chris Purdy on 09/03/2020.
//

#ifndef KSP_2D_CW4_FASTVEC2D_H
#define KSP_2D_CW4_FASTVEC2D_H

#include <iostream>
#include <string>
#include <cmath>
#include "../../sdl2_framework/SimpleTrig.h"
#include "Vec2D.h"

class FastVec2D {
public:
    FastVec2D(double X = 0, double Y = 0)
    {
        x = X;
        y = Y;
        fastTrig = SimpleTrig::get();
    };

    FastVec2D(Vec2D& vec2D)
    {
        x = vec2D.x;
        y = vec2D.y;
        fastTrig = SimpleTrig::get();
    };

    FastVec2D& operator=(const Vec2D& vec2D) {
        x = (double)vec2D.x;
        y = (double)vec2D.y;
        return *this;
    }

    ~FastVec2D() {} ;

    double x, y;

    FastVec2D operator*(double scalar) const
    {
        return FastVec2D(x * scalar, y * scalar);
    }

    FastVec2D operator*=(double scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }


    FastVec2D operator+(const FastVec2D &vect) const
    {
        return FastVec2D(x + vect.x, y + vect.y);
    }

    FastVec2D& operator+=(const FastVec2D &vect)
    {
        x += vect.x;
        y += vect.y;
        return *this;
    }

    FastVec2D operator-(const FastVec2D &vect) const
    {
        return FastVec2D(x - vect.x, y - vect.y);
    }

    FastVec2D operator-=(const FastVec2D &vect)
    {
        x -= vect.x;
        y -= vect.y;
        return *this;
    }

    /* this actually rotates using degrees which isn't great since Vec2D uses radians, but
     * thats fine for small increments and decrements of the angle
     *
     * (UPDATE uses radians now)
     */
    FastVec2D& rotate(float angle)
    {
//        angle *= 10 * (180/M_PI);
//        double cosAngle = fastTrig->cos(angle);
//        double sinAngle = fastTrig->sin(angle);
        double cosAngle = cosf(angle);
        double sinAngle = sinf(angle);
        double xt = (x * cosAngle) - (y * sinAngle);
        double yt = (y * cosAngle) + (x * sinAngle);
        x = xt;
        y = yt;
        return *this;
    }

    FastVec2D rotateCopy(float angle)
    {
        double cosAngle = cosf(angle);
        double sinAngle = sinf(angle);
        double xt = (x * cosAngle) - (y * sinAngle);
        double yt = (y * cosAngle) + (x * sinAngle);
        return FastVec2D(xt, yt);
    }

    double crossproduct(const FastVec2D &vect2) const
    {
        return (this->x * vect2.y) - (this->y * vect2.x);
    }

    float magnitude()
    {
        return sqrtf(x * x + y * y);
    }

    FastVec2D& normalise()
    {
        float mag = sqrtf(x* x + y * y);
        this->x = x / mag;
        this->y = y / mag;
        return *this;
    }

    // return dot product
    float dotproduct(const FastVec2D &vect) const
    {
        return (x * vect.x) + (y * vect.y);
    }

    std::string to_string() {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

    SimpleTrig* fastTrig;
};

#endif //KSP_2D_CW4_FASTVEC2D_H