#include "PhysicsSystem.h"

#include <Fission/Core/Math.h>
#include <Fission/Rendering/TransformComponent.h>

#include "phys/Collision.h"
#include "phys/CollisionDispatcher.h"

#include "TetraCollision.h"
#include "GridSystem.h"
#include "GridComponent.h"
#include "PhysicsComponent.h"

PhysicsSystem::PhysicsSystem(EventManager *eventManager, float lockStep) :
    System(eventManager, lockStep, TransformComponent::Type|PhysicsComponent::Type, 0)
{
    world = new phys::PhysicsWorld(lockStep, 10);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Circle, phys::Shape::Circle, phys::circleToCircle);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Circle, phys::Shape::Polygon, phys::circleToPolygon);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Polygon, phys::Shape::Circle, phys::polygonToCircle);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Polygon, phys::Shape::Polygon, phys::polygonToPolygon);
    phys::CollisionDispatcher::registerCallback(Shape::Grid, phys::Shape::Polygon, gridToPolygon);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Polygon, Shape::Grid, polygonToGrid);
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

        phys->getBody()->setPosition(trans->getPosition()/PTU);
        phys->getBody()->setRotation(trans->getRotation());
    }

    world->step();
}

void PhysicsSystem::processEntity(Entity *entity, const float dt)
{
    auto trans = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    auto phys = reinterpret_cast<PhysicsComponent*>(entity->getComponent(PhysicsComponent::Type));

    trans->setPosition(phys->getBody()->getPosition()*PTU);
    trans->setRotation(radToDeg(phys->getBody()->getRotation()));
}

void PhysicsSystem::end(const float dt)
{
}

void PhysicsSystem::onEntityAdded(Entity* entity)
{
    auto phys = reinterpret_cast<PhysicsComponent*>(entity->getComponent(PhysicsComponent::Type));
    world->addRigidBody(phys->getBody());
}

void PhysicsSystem::onEntityRemoved(Entity* entity)
{
    auto phys = reinterpret_cast<PhysicsComponent*>(entity->getComponent(PhysicsComponent::Type));
    world->removeRigidBody(phys->getBody());
}
