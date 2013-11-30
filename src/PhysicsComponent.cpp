#include "PhysicsComponent.h"

#include "phys/Shape.h"

#include "PhysicsSystem.h"
#include "GridComponent.h"
#include "TetraCollision.h"

TypeBits PhysicsComponent::Type;

PhysicsComponent::PhysicsComponent(PhysicsSystem* physSys, int width, int height) : grid(NULL)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->setBox((width/2)/PTU, (height/2)/PTU);
    body = new phys::RigidBody(shape, 1.f);
    physSys->getWorld()->addRigidBody(body);
    body->setFixedRotation();
}

PhysicsComponent::PhysicsComponent(PhysicsSystem* physSys, GridComponent* gridCmp) : grid(NULL)
{
    GridShape* shape = new GridShape(gridCmp);
    body = new phys::RigidBody(shape, 1.f);
    body->setStatic();
    physSys->getWorld()->addRigidBody(body);
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
