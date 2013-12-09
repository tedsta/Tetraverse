#ifndef COLLISION_H
#define COLLISION_H

namespace phys
{
    class Collision;
    class RigidBody;

    void circleToCircle(Collision* c, RigidBody* a, RigidBody* b);
    void circleToPolygon(Collision* c, RigidBody* a, RigidBody* b);
    void polygonToCircle(Collision* c, RigidBody* a, RigidBody* b);
    void polygonToPolygon(Collision* c, RigidBody* a, RigidBody* b);
}

#endif // COLLISION_H
