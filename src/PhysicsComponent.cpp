#include "PhysicsComponent.h"

#include "GridComponent.h"

TypeBits PhysicsComponent::Type;

PhysicsComponent::PhysicsComponent(int width, int height) : mGravityDir(DOWN), mPrimaryGrid(NULL),
    mWidth(width), mHeight(height)
{
    //ctor
}

PhysicsComponent::~PhysicsComponent()
{
    //dtor
}

void PhysicsComponent::serialize(sf::Packet &packet)
{
}

void PhysicsComponent::deserialize(sf::Packet &packet)
{
}
