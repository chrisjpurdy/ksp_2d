//
// Created by Chris Purdy on 03/04/2020.
//

#include <vector>
#include "PhysObject.h"
#include "Projection.h"

void PhysObject::setScreenOrigin(Vec2D* origin) {
    screenViewOrigin = origin;
}

void PhysObject::setDistModifier(const long double& dist) {
    distMult = dist;
}

void PhysObject::setTimeModifier(const long double& time) {
    timeMult = time;
}

void PhysObject::updateScreenOrient() {
    screenOrient = body->orient + screenOrientOffset;
    screenOrientMatrix = body->orientMatrix * screenOrientOffsetMatrix;
}

void PhysObject::updateScreenOrient(double newOffset) {
    screenOrientOffset = newOffset;
    screenOrientOffsetMatrix = Mat22(screenOrientOffset);
    updateScreenOrient();
}

//void PhysObject::calculateMass() {
//    if(body->mass_data.mass == 0)
//        body->mass_data.inv_mass = 0;
//    else
//        body->mass_data.inv_mass = 1 / body->mass_data.mass;
//}

void PhysObject::recalcScreenPos(const Vec2D& pos) {
    screenPosition = pos;
    screenPosition -= *screenViewOrigin;
    screenPosition *= 1.0 / distMult;
    screenPosition += screenCenter;
}

void PhysObject::recalcScreenPos() {
    screenPosition = body->position;
    screenPosition -= *screenViewOrigin;
    screenPosition *= 1.0 / distMult;
    screenPosition += screenCenter;
}

void PhysObject::tick(int iCurrentTime) {

    if (iPreviousTime == -1) {
        iPreviousTime = iCurrentTime;
        return;
    }

    /* if more than a tenth of a second has passed since the last tick, don't allow this tick to
     * modify the forces on the gravity objects (since this may throw off objects drastically
     * from their proper orbits)
     */
    if ((iCurrentTime - iPreviousTime) > 100) {
        std::cerr << "Lagging - stopping gravity object calculations" << std::endl;
        iPreviousTime = iCurrentTime;
        return;
    }

    long double timeSlice = ((iCurrentTime - iPreviousTime) / 1000.0) * timeMult;

    body->updatePosition(timeSlice);
    body->clearForce();

    recalcShape();
    recalcScreenPos();

    iPreviousTime = iCurrentTime; // update previous tick time (in milliseconds)
    body->recalcForce = true;
}

void PhysObject::recalcShape() {
    shape->updateShape(body->position, body->orientMatrix);
}

PhysObject::PhysObject(Vec2D *_screenViewOrigin) : screenViewOrigin(_screenViewOrigin), iPreviousTime(-1) {
    distMult = 1000000;
    timeMult = 1;
}

PhysObject::~PhysObject() {
    delete body;
    delete shape;
}


/* ----- PhysBody ----- */

const double PhysBody::TWO_PI = 2*M_PI;
const double PhysBody::HALF_PI = M_PI/2.0;
const long double PhysBody::G(6.674 * pow(10,-11));

void PhysBody::resolveCollision(const CollisionManifold& manifold) {

    PhysObject *A = manifold.A;
    PhysObject *B = manifold.B;

    Vec2D relativeVel = B->body->velocity - A->body->velocity;
    long double velAlongNormal = relativeVel.dotproduct(manifold.normal);

    Vec2D normalPerp = manifold.normal.perpendicularClockwise();

    if(velAlongNormal > 0)
        return;

    double e = std::min( A->body->material.restitution, B->body->material.restitution);

    long double j = -(1 + e) * velAlongNormal;
    j /= A->body->mass_data.inv_mass + B->body->mass_data.inv_mass;

    Vec2D impulse = manifold.normal * j;

    // applying friction perpendicular to the surface normal (ONLY TAKING INTO ACCOUNT OBJECT A CURRENTLY!!!)
    long double velAlongPerp = relativeVel.dotproduct(normalPerp);
    impulse += normalPerp * (-velAlongPerp / A->body->mass_data.inv_mass) * 0.5;

    //impulse += manifold.normal.perpendicularClockwise() * (-velAlongPerp / A->body->mass_data.inv_mass);

    if (!A->body->unmoveableBody) {
        A->body->applyForce(-impulse);
        // calculate amount to change rotation by
        long double collisionCentreDist = normalPerp.dotproduct(A->body->position) - normalPerp.dotproduct(manifold.collisionPoint);
        //std::cout << collisionCentreDist << std::endl;
        A->body->applyRotation(-collisionCentreDist/1000.0);
        A->body->angularForce *= 0.9;
        //A->body->rotate(-collisionCentreDist/10000.0);
        A->body->position -= manifold.normal * manifold.penetration;
    }

    if (!B->body->unmoveableBody) {
        B->body->applyForce(impulse);
        B->body->position += manifold.normal * manifold.penetration;
    }
}

// TODO create polygon circle SAT collision resolution algorithm

// Calculates the collision penetration, normal, and if a collision has occurred - uses separating axis theorem (SAT)
bool PhysBody::OBBCircleCollision(CollisionManifold& manifold) {

    auto* obb = reinterpret_cast<OBB*>(manifold.A->shape);
    auto* circle = reinterpret_cast<Circle*>(manifold.B->shape);

    std::vector<Vec2D> axes;
    std::vector<Vec2D> obbPoints;
    obbPoints.push_back(obb->topLeft);
    obbPoints.push_back(obb->topRight);
    obbPoints.push_back(obb->bottomRight);
    obbPoints.push_back(obb->bottomLeft);

    // find closest point on bounding box to circle to determine 5th axis to check during SAT algorithm
    long double minSquaredDist = INFINITY;
    int closestPointIndex;
    long double pointDistSquared;
    for (int i=0; i<4; ++i) {
        pointDistSquared = (obbPoints[i] - circle->position).magnitudeSquared();
        if (pointDistSquared < minSquaredDist) {
            closestPointIndex = i;
            minSquaredDist = pointDistSquared;
        }
    }

    axes.push_back(obbPoints[closestPointIndex] - circle->position);
    axes.push_back((obbPoints[1] - obbPoints[0]).perpendicularClockwise());
    axes.push_back((obbPoints[2] - obbPoints[1]).perpendicularClockwise());
    axes.push_back((obbPoints[3] - obbPoints[2]).perpendicularClockwise());
    axes.push_back((obbPoints[0] - obbPoints[3]).perpendicularClockwise());

    Projection shadowA(0,0);
    Projection shadowB(0,0);
    manifold.penetration = INFINITY;
    // calculate whether each set of projections on each axis are intersecting
    for (auto & axis : axes) {
        // set up the axis properly (needs to be normalised)
        axis.normalise();
        shadowA = Projection::projectPolygon(axis, obbPoints);
        shadowB = Projection::projectCircle(axis, circle->position, circle->radius);

        // if separated on one major axis then definitely not overlapping
        if (!shadowA.isOverlapping(shadowB)) {
            return false;
        }

        // now see if it is the smallest overlap
        long double o = shadowA.getOverlap(shadowB);

        // deal with containment
        if (shadowA.contains(shadowB) || shadowB.contains(shadowA)) {
            // get the overlap plus the distance from the minimum end points
            long double mins = abs(shadowA.minPoint - shadowB.minPoint);
            long double maxs = abs(shadowA.maxPoint - shadowB.maxPoint);
            if (mins < maxs) {
                o += mins;
            } else {
                o += maxs;
            }
        }

        // if so set the manifold normal and penetration
        if (o < manifold.penetration) {
            manifold.penetration = o;
            manifold.normal = -axis;
        }
    }

    // if we get here then all projections on all axes overlap, so the shapes are definitely overlapping by SAT
    manifold.collisionPoint = obbPoints[closestPointIndex];
    return true;

}

PhysBody::PhysBody(PhysObject* _object, long double mass, const Vec2D& initialPosition, const Vec2D& initialVelocity)
    : object(_object), accuTimeSlice(0), mass_data(mass, 1.0/mass, 0, 0), position(initialPosition),
    velocity(initialVelocity), angularVelocity(0), recalcForce(true), unmoveableBody(false), force(0), angularForce(0),
    closeByBody(false) {
    object->recalcScreenPos(position);
}


/* ----- RailedPhysBody ----- */

void RailedPhysBody::updatePosition(long double timeSlice) {

    if (!parent) return;

    accuTimeSlice += timeSlice;

    // if the accumulated time is more than 1 seconds, then allow for the calculation to happen
    if (accuTimeSlice < 1 && !closeByBody) return;

    prevPos = position;

    //std::cout << "Accumulated enough time: " << accuTimeSlice << std::endl;

    double rotateAmount = (accuTimeSlice/timeForOrbit) * TWO_PI;
    meanAnomaly += rotateAmount;
    meanAnomaly = meanAnomaly >= TWO_PI ? meanAnomaly - TWO_PI : meanAnomaly;

    double angle = keplersEqnSolve();

    parentRelativePos.x = semiMajorAxis * (cos(angle) - eccentricity);
    parentRelativePos.y = semiMajorAxis * sqrt1MinESqr * sin(angle);

    position = parentRelativePos + parent->position;

    velocity = (position - prevPos) * (1.0/accuTimeSlice);

    // reset accumulation after calculating
    accuTimeSlice = 0;
}

double RailedPhysBody::keplersEqnSolve() {
    double u = meanAnomaly;
    double uPrev = 0;

    do {
        uPrev = u;
        u = meanAnomaly + (eccentricity * ::sin(u));
    } while (abs(uPrev - u) > 0.001);

    return u;
}

RailedPhysBody::RailedPhysBody(PhysObject* object, long double mass, const Vec2D& initialPos, const Vec2D& initalVel, PhysBody *parentBody,
                               double _eccentricity, long double _timeForOrbit, double initialAngleOffset)
                               : PhysBody(object, mass, initialPos, initalVel), parent(parentBody), eccentricity(_eccentricity), eSquared(pow(_eccentricity,2)),
                               sqrt1MinESqr(::sqrt(1 - eSquared)), timeForOrbit(_timeForOrbit), prevPos(position)  {
    parentRelativePos = position;
    meanAnomaly = initialAngleOffset;
    semiMajorAxis = parentRelativePos.magnitude();
    updatePosition(1);
}