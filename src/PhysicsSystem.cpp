#include "PhysicsSystem.h"

#include <Fission/Core/Math.h>
#include <Fission/Rendering/TransformComponent.h>

#include "GridSystem.h"
#include "GridComponent.h"
#include "PhysicsComponent.h"

PhysicsSystem::PhysicsSystem(EventManager *eventManager) :
    System(eventManager, TransformComponent::Type|PhysicsComponent::Type, 0)
{
    world = new phys::PhysicsWorld(0.016f, 10);
}

PhysicsSystem::~PhysicsSystem()
{
    delete world;
}

void PhysicsSystem::begin(const float dt)
{
    for (auto entity : getActiveEntities())
    {
        auto trans = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
        auto phys = reinterpret_cast<PhysicsComponent*>(entity->getComponent(PhysicsComponent::Type));

        phys->getBody()->setPosition(trans->getPosition());
        phys->getBody()->setRotation(trans->getRotation());
    }

    world->step();
}

void PhysicsSystem::processEntity(Entity *entity, const float dt)
{
    auto trans = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    auto phys = reinterpret_cast<PhysicsComponent*>(entity->getComponent(PhysicsComponent::Type));

    trans->setPosition(phys->getBody()->getPosition());
    trans->setRotation(phys->getBody()->getRotation());
}

void PhysicsSystem::end(const float dt)
{
}
