#include "PhysicsComponent.h"

#include "phys/Shape.h"

#include "PhysicsSystem.h"
#include "GridComponent.h"
#include "TetraCollision.h"

TypeBits PhysicsComponent::Type;

PhysicsComponent::PhysicsComponent(PhysicsSystem* physSys, float width, float height) : grid(NULL)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->setBox((width/2.f)/PTU, (height/2.f)/PTU);
    body = new phys::RigidBody(shape, 1.f);
    physSys->getWorld()->addRigidBody(body);
    body->setFixedRotation();
}

PhysicsComponent::PhysicsComponent(PhysicsSystem* physSys, GridComponent* gridCmp) : grid(NULL)
{
    GridShape* shape = new GridShape(gridCmp);
    body = new phys::RigidBody(shape, 1.f);
    physSys->getWorld()->addRigidBody(body);

    if (gridCmp->getWrapX())
        body->setStatic();
}

PhysicsComponent::~PhysicsComponent()
{
    delete body;
}

void PhysicsComponent::serialize(sf::Packet &packet)
{
}

void PhysicsComponent::deserialize(sf::Packet &packet)
{
}
