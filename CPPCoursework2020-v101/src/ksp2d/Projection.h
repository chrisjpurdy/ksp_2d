//
// Created by Chris Purdy on 14/04/2020.
//

#ifndef KSP_2D_CW4_PROJECTION_H
#define KSP_2D_CW4_PROJECTION_H

#include "Vec2D.h"

class Projection {
public:
    long double minPoint;
    long double maxPoint;

    Projection(const long double& min, const long double& max) : minPoint(min), maxPoint(max) {}

    void set(const long double& min, const long double& max) {
        minPoint = min;
        maxPoint = max;
    }

    long double getOverlap(Projection& other) {
        return std::max((long double)0, std::min(this->maxPoint, other.maxPoint) - std::max(this->minPoint, other.minPoint));
    }

    bool isOverlapping(Projection& other) {
        return getOverlap(other) > 0;
    }

    bool contains(Projection& other) {
        return (minPoint < other.minPoint) && (maxPoint > other.maxPoint);
    }

    // axis needs to be a normalised vector
    static Projection projectPolygon(const Vec2D& axis, const std::vector<Vec2D>& vertices) {
        long double min = axis.dotproduct(vertices[0]);
        long double max = min;
        for (int i = 1; i < vertices.size(); ++i) {
            // NOTE: the axis must be normalized to get accurate projections
            long double p = axis.dotproduct(vertices[i]);
            if (p < min) {
                min = p;
            } else if (p > max) {
                max = p;
            }
        }
        return {min, max};
    }

    // axis needs to be a normalised vector
    static Projection projectCircle(const Vec2D& axis, const Vec2D& centre, long double radius) {
        Vec2D edgePoint1 = centre + axis * radius;
        Vec2D edgePoint2 = centre + (-axis) * radius;
        long double p = axis.dotproduct(edgePoint1);
        long double q = axis.dotproduct(edgePoint2);
        return {std::min(p, q), std::max(p, q)};
    }
};

#endif //KSP_2D_CW4_PROJECTION_H
