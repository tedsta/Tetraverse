#include "Components/RigidBody.h"

#include <cfloat>

#include "phys/Shape.h"

#include "Systems/PhysicsSystem.h"

RigidBody::RigidBody(int width, int height, float density) : mDensity(density)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->setBox((float(width)/2.f)/PTU, (float(height)/2.f)/PTU);
    mBody = new phys::RigidBody(shape, 2, density);
    mBody->setGravity(sf::Vector2f(0, 40.f));
    mBody->setFixedRotation();
}

RigidBody::RigidBody(float radius, float density) : mDensity(density)
{
    phys::CircleShape* shape = new phys::CircleShape(radius);
    mBody = new phys::RigidBody(shape, 2, density);
    mBody->setGravity(sf::Vector2f(0, 40.f));
    mBody->setFixedRotation();
    mBody->setRestitution(0.f);
    mBody->setDynamicFriction(0.f);
    mBody->setStaticFriction(0.f);
}

RigidBody::RigidBody(sf::Vector2f* verts, int vertCount, float density) : mDensity(density)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->set(verts, vertCount);
    mBody = new phys::RigidBody(shape, 2, density);
    mBody->setGravity(sf::Vector2f(0, 40.f));
    mBody->setFixedRotation();
}

RigidBody::~RigidBody()
{
    delete mBody;
}

void RigidBody::serialize(sf::Packet &packet)
{
    packet << mBody->getShape()->getType();
    mBody->getShape()->serialize(packet);

    packet << mBody->getType();
    packet << mDensity;
    packet << mBody->getGravity().x << mBody->getGravity().y;
}

void RigidBody::deserialize(sf::Packet &packet)
{
    phys::Shape* shape;

    int shapeType;
    packet >> shapeType;
    switch (shapeType)
    {
    case phys::Shape::Circle:
        {
            shape = new phys::CircleShape(0);
            break;
        }
    case phys::Shape::Polygon:
        {
            shape = new phys::PolygonShape;
            break;
        }
    default:
        {
            return;
        }
    }

    shape->deserialize(packet);

    int type;
    sf::Vector2f gravity;
    packet >> type;
    packet >> mDensity;
    packet >> gravity.x >> gravity.y;

    mBody = new phys::RigidBody(shape, type, mDensity);
    mBody->setGravity(gravity);

    switch (shapeType)
    {
    case phys::Shape::Circle:
    case phys::Shape::Polygon:
        {
            mBody->setFixedRotation();
            break;
        }
    }
}
