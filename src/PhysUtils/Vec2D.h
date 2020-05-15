//
// Created by Chris Purdy on 09/03/2020.
//

#ifndef KSP_2D_CW4_VEC2D_H
#define KSP_2D_CW4_VEC2D_H

#include <iostream>
#include <string>
#include <cmath>

class Vec2D {
public:
    Vec2D(long double X = 0, long double Y = 0)
    {
        x = X;
        y = Y;
    };

    ~Vec2D() {} ;

    long double x, y;

    Vec2D operator*(long double scalar) const
    {
        return Vec2D(x * scalar, y * scalar);
    }

    Vec2D operator*=(long double scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }


    Vec2D operator+(const Vec2D &vect) const
    {
        return Vec2D(x + vect.x, y + vect.y);
    }

    Vec2D& operator+=(const Vec2D &vect)
    {
        x += vect.x;
        y += vect.y;
        return *this;
    }

    Vec2D operator-(const Vec2D &vect) const
    {
        return Vec2D(x - vect.x, y - vect.y);
    }

    Vec2D operator-() const
    {
        return Vec2D(-x, -y);
    }

    Vec2D operator-=(const Vec2D &vect)
    {
        x -= vect.x;
        y -= vect.y;
        return *this;
    }

    bool operator==(const Vec2D &vect)
    {
        return (x == vect.x) && (y == vect.y);
    }

    Vec2D& set(long double _x, long double _y)
    {
        x = _x;
        y = _y;
        return *this;
    }

    Vec2D& rotate(double angle)
    {
        long double xt = (x * cos(angle)) - (y * sin(angle));
        long double yt = (y * cos(angle)) + (x * sin(angle));
        x = xt;
        y = yt;
        return *this;
    }

    Vec2D rotateCopy(double angle) const
    {
//        long double xt = (x * cos(angle)) - (y * sin(angle));
//        long double yt = (y * cos(angle)) + (x * sin(angle));
        return Vec2D(x, y).rotate(angle);
    }

    long double angleBetween(const Vec2D& vect2) const {
        return acos(dotproduct(vect2)/(magnitude() * vect2.magnitude()));
    }

    long double crossproduct(const Vec2D &vect2) const
    {
        return (this->x * vect2.y) - (this->y * vect2.x);
    }

    long double magnitude() const
    {
        return sqrt(x * x +y * y);
    }

    long double magnitudeSquared() const
    {
        return x * x + y * y;
    }

    Vec2D& normalise()
    {
        long double mag = sqrt(x * x + y * y);
        this->x = x / mag;
        this->y = y / mag;
        return *this;
    }

    // return vector perpendicular to this one (rotated clockwise)
    Vec2D perpendicularClockwise() const
    {
        return Vec2D(y, -x);
    }

    // return vector perpendicular to this one (rotated anti-clockwise)
    Vec2D perpendicularAClockwise() const
    {
        return Vec2D(-y, x);
    }

    // return dot product
    long double dotproduct(const Vec2D &vect) const
    {
        return (x * vect.x) + (y * vect.y);
    }

    std::string to_string() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

#endif //KSP_2D_CW4_VEC2D_H
