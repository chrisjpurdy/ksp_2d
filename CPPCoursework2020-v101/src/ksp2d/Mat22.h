//
// Created by Chris Purdy on 08/04/2020.
//

#ifndef KSP_2D_CW4_MAT22_H
#define KSP_2D_CW4_MAT22_H

#include "Vec2D.h"

/**
 * A class to handle 2x2 matricies
 *  | m00 m01 |
 *  | m10 m11 |
 */
class Mat22 {
public:

    long double m00, m01, m10, m11;

    Mat22() {
        m00 = m01 = m10 = m11 = 0;
    }

    Mat22(long double _m00, long double _m01, long double _m10, long double _m11) {
        m00 = _m00; m01 = _m01;
        m10 = _m10; m11 = _m11;
    }

    // clockwise rotation
    explicit Mat22(double radians) {
        double c = std::cos( radians );
        double s = std::sin( radians );

        m00 =  c; m01 = s;
        m10 = -s; m11 = c;
    }

    Mat22( const Vec2D& xcol, const Vec2D& ycol )
    {
        m00 = xcol.x;
        m10 = xcol.y;
        m01 = ycol.x;
        m11 = ycol.y;
    }

    Mat22 inverted() const {
        auto invMatrix = Mat22(m11, -m01, -m10, m00);
        invMatrix *= (1.0/(m00*m11 - m01*m10));
        return invMatrix;
    }

    Vec2D operator*(const Vec2D& vec) const {
        return Vec2D( m00 * vec.x + m01 * vec.y, m10 * vec.x + m11 * vec.y );
    }

    Mat22 operator*(const Mat22& rhs) const {
        Mat22 matrix;

        matrix.m00 = m00 * rhs.m00 + m01 * rhs.m10;   matrix.m01 = m00 * rhs.m01 + m01 * rhs.m11;
        matrix.m10 = m10 * rhs.m00 + m11 * rhs.m10;   matrix.m11 = m10 * rhs.m01 + m11 * rhs.m11;
        return matrix;
    }

    Mat22& operator*=(const Mat22& rhs) {
        Mat22 matrix;

        matrix.m00 = m00 * rhs.m00 + m01 * rhs.m10;   matrix.m01 = m00 * rhs.m01 + m01 * rhs.m11;
        matrix.m10 = m10 * rhs.m00 + m11 * rhs.m10;   matrix.m11 = m10 * rhs.m01 + m11 * rhs.m11;

        m00 = matrix.m00; m01 = matrix.m01;
        m10 = matrix.m10; m11 = matrix.m11;
        return *this;
    }

    Mat22& operator*=(long double scalar) {
        m00 *= scalar; m01 *= scalar;
        m10 *= scalar; m11 *= scalar;
        return *this;
    }
};

#endif //KSP_2D_CW4_MAT22_H
