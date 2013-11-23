#ifndef COLLISION_H
#define COLLISION_H

namespace phys
{
    class Manifold;
    class RigidBody;

    void circleToCircle(Manifold* m, RigidBody* a, RigidBody* b);
    void circleToPolygon(Manifold* m, RigidBody* a, RigidBody* b);
    void polygonToCircle(Manifold* m, RigidBody* a, RigidBody* b);
    void polygonToPolygon(Manifold* m, RigidBody* a, RigidBody* b);
}

#endif // COLLISION_H
