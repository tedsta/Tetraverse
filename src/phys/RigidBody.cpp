#include "phys/RigidBody.h"

#include "phys/Shape.h"

namespace phys
{
    RigidBody::RigidBody(Shape* _shape, float density) : shape(_shape)
    {
        angularVelocity = 0;
        torque = 0;
        staticFriction = 0.8f;
        dynamicFriction = 0.55f;
        restitution = 0.2f;
        shape->computeMass(this, density);
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

