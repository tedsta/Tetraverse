#include "phys/RigidBody.h"

#include "phys/Shape.h"

namespace phys
{
    RigidBody::RigidBody(Shape* _shape, int _type, float density) : type(_type), shape(_shape), parent(NULL)
    {
        angularVelocity = 0;
        torque = 0;
        staticFriction = 0.8f;
        dynamicFriction = 0.55f;
        restitution = 0.2f;
        shape->computeMass(this, density);
        mass_ = mass;
        inverseMass_ = inverseMass;
        inertia_ = inertia;
        inverseInertia_ = inverseInertia;
    }

    RigidBody::~RigidBody()
    {
        delete shape;
    }

    void RigidBody::setRotation(float rot)
    {
        rotation = rot;
        shape->setRotation(rot);
    }
}

