//
// Created by Chris Purdy on 03/04/2020.
//

#ifndef KSP_2D_CW4_PHYSOBJECT_H
#define KSP_2D_CW4_PHYSOBJECT_H


#include "Vec2D.h"
#include "../KSP.h"
#include "Mat22.h"

struct MassData
{
    MassData(long double m, long double im, long double i, long double ii)
    : mass(m), inv_mass(im), inertia(i), inverse_inertia(ii) {}

    long double mass;
    long double inv_mass;

    // For rotations (not covered in this article)
    long double inertia;
    long double inverse_inertia;
};

struct Material
{
    double density;
    double restitution;
};

/* Stores information about the collision boundary of a body
 */
class CollisionShape {
public:
    virtual void updateShape(const Vec2D& pos, const Mat22& rot){};
};

class OBB : public CollisionShape {
public:
    OBB(const Vec2D& tl, const Vec2D& bl, const Vec2D& center, const Mat22& rot) {
        // TODO need to set these with respect to rotation (for now all OBBs assumed to start with 0 rotation)
        centerToTopLeft = tl - center;
        centerToBottomLeft = bl - center;
        updateShape(center, rot);
        //std::cout << "Cent to tl: " << centerToTopLeft.to_string() << ", cent to br: " << centerToBottomRight.to_string() << std::endl;
    }

    void updateShape(const Vec2D& pos, const Mat22& rot) override {
        rotation = rot;
        inverseRotation = rot.inverted();
        topLeft = pos + (rotation * centerToTopLeft);
        bottomRight = pos + (rotation * -centerToTopLeft);
        topRight = pos + (rotation * -centerToBottomLeft);
        bottomLeft = pos + (rotation * centerToBottomLeft);
//        std::cout << "tl: " << topLeft.to_string() << std::endl;
//        std::cout << "br: " << bottomRight.to_string() << std::endl;
    }

    Vec2D topLeft;
    Vec2D bottomRight;
    Vec2D topRight;
    Vec2D bottomLeft;
    Mat22 rotation;
    Mat22 inverseRotation;

    Vec2D centerToTopLeft;
    Vec2D centerToBottomLeft;
};

class Circle : public CollisionShape {
public:
    Circle(long double r, const Vec2D& p) : radius(r), position(p) {}

    void updateShape(const Vec2D& pos, const Mat22& rot) override {
        position = pos;
    }

    long double radius;
    Vec2D position;
};

class PhysBody;

/* Contains a physics body, and wraps it in values to contain screen positional values
 * also contains a collision shape to define its collision boundary
 */
class PhysObject {
public:

    explicit PhysObject(Vec2D *_screenViewOrigin);

    ~PhysObject();

    // overriding functions need to call the super function, or recalculate inverse mass and inertia themselves
    //virtual void calculateMass();

    // shape and body should be setup by subclass
    PhysBody *body = nullptr;
    CollisionShape *shape = nullptr;

    void updateScreenOrient();
    void updateScreenOrient(double newOffset);

    Vec2D screenPosition;
    Vec2D* screenViewOrigin;
    double screenOrient{}; // radians
    Mat22 screenOrientMatrix;
    double screenOrientOffset{};
    Mat22 screenOrientOffsetMatrix;
    long double distMult;
    long double timeMult;

    void setScreenOrigin(Vec2D* origin);
    void setDistModifier(const long double& dist);
    void setTimeModifier(const long double& time);

    void recalcScreenPos();
    void recalcScreenPos(const Vec2D &pos);

    int iPreviousTime;
    void tick(int iCurrentTime);

    void recalcShape();
};

/* Used to store data about a collision
 * the physics objects store the shape used to calculate the penetration and normal
 */
struct CollisionManifold
{
    CollisionManifold(PhysObject *a, PhysObject *b) : A(a), B(b) {}

    PhysObject *A;
    PhysObject *B;
    long double penetration;
    Vec2D normal;
    Vec2D collisionPoint;

};

// Contains all values and logic for physics calculations
class PhysBody {
public:

    PhysBody(PhysObject *_object, long double mass, const Vec2D& initialPosition, const Vec2D& initialVelocity);

    PhysObject *object;

    Material material;
    MassData mass_data;
    bool unmoveableBody;
    bool closeByBody;

    static const long double G;
    static const double TWO_PI;
    static const double HALF_PI;

    Vec2D position;

    long double angularVelocity;
    long double angularForce;
    double torque;
    double orient; // radians
    Mat22 orientMatrix; // a matrix transformation representing the orientation angle

    Vec2D velocity;
    Vec2D force;

    long double accuTimeSlice;
    bool recalcForce;

    void applyForce(const Vec2D &f) {
        force += f;
    }

    void applyRotation(long double amt) {
        angularForce += amt;
        //std::cout << angularForce << std::endl;
    }

    void applyForceFromDir(double angle, double magnitude) {

        if (!recalcForce || mass_data.mass == 0) return;

        Vec2D r = Vec2D(0,1);
        r.rotate(angle);

        force += r * magnitude;
    }

    void applyForceFromDir(Mat22 rotMatrix, double magnitude) {

        if (!recalcForce || mass_data.mass == 0) return;

        Vec2D r = rotMatrix * Vec2D(0,1);

        force += r * magnitude;
    }

    void applyGravity(PhysBody *other) {

        // if force isn't yet used or object is static
        if (!recalcForce || mass_data.mass == 0) return;

        Vec2D r = (other->position - position);
        // F = G(m1*m2)/(r^2), F1=m1a1 -> a1 = G(m2)/(r^2)
        long double f = G * ((mass_data.mass * other->mass_data.mass) / (r.x * r.x + r.y * r.y));

        // now convert acc to a vector
        force += r.normalise() * f;
    }

    // to be used at the end of calculation of the new velocity this frame
    void clearForce()
    {
        force = 0;
        angularForce = 0;
    }

    // application of impulse taking into account rotation
//    void applyImpulse(const Vec2D& impulse, const Vec2D& contactVector)
//    {
//        velocity += impulse * mass_data.inv_mass;
//        angularVelocity += contactVector.crossproduct(impulse) * mass_data.inverse_inertia;
//    }

    void applyImpulse(long double timeSlice) {
        velocity += force * timeSlice * mass_data.inv_mass;
        angularVelocity += angularForce * timeSlice;
    }

    void setStatic() {
        unmoveableBody = true;
    }

    void setOrient(double radians) {
        orient = radians;
        orientMatrix = Mat22(orient);
        object->updateScreenOrient();
    }

    void rotate(double radians) {
        orient += radians;
        orientMatrix = Mat22(orient);
        object->updateScreenOrient();
    }
//    void setOrientMatrix(const Mat22& rotMatrix) {
//        orientMatrix = rotMatrix;
//        object->updateScreenOrient();
//    }

    // Overridden to change how the position is changed on each tick
    virtual void updatePosition(long double timeSlice) {
        accuTimeSlice += timeSlice;
        applyImpulse(accuTimeSlice);
        position += velocity * accuTimeSlice;
        rotate(angularVelocity * accuTimeSlice);
        accuTimeSlice = 0;
    }

    /* ---- Static functions that act on two bodies etc. ---- */

    static void resolveCollision(const CollisionManifold& manifold);

    static bool OBBCircleCollision(CollisionManifold& manifold);

};

class RailedPhysBody : public PhysBody {
public:

    RailedPhysBody(PhysObject *object, long double mass, const Vec2D& initialPos, const Vec2D& initalVel, PhysBody *parentBody,
                   double _eccentricity, long double _timeForOrbit, double _initialAngleOffset);

    Vec2D prevPos;

    PhysBody* parent;
    Vec2D parentRelativePos;

    double eccentricity;
    double eSquared;
    double sqrt1MinESqr;
    long double semiMajorAxis;

    long double timeForOrbit;
    double meanAnomaly;

    void updatePosition(long double timeSlice) override;

    double keplersEqnSolve();

};


#endif //KSP_2D_CW4_PHYSOBJECT_H