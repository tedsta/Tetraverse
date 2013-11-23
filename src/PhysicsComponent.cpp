#include "PhysicsComponent.h"

#include "phys/Shape.h"

#include "PhysicsSystem.h"
#include "GridComponent.h"

TypeBits PhysicsComponent::Type;

PhysicsComponent::PhysicsComponent(PhysicsSystem* physSys, int width, int height) : grid(NULL)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->setBox(width/2, height/2);
    body = new phys::RigidBody(shape, 1.f);
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
