#include "Systems/PhysicsSystem.h"

#include <Fission/Core/Math.h>
#include <Fission/Rendering/Transform.h>

#include "phys/Collision.h"
#include "phys/CollisionDispatcher.h"

#include "Components/RigidBody.h"

PhysicsSystem::PhysicsSystem(fsn::IEventManager* eventManager, float lockStep) :
    System(eventManager, lockStep)
{
    mAspect.all<fsn::Transform, RigidBody>();

    mWorld = new phys::PhysicsWorld(lockStep, 10);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Circle, phys::Shape::Circle, phys::circleToCircle);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Circle, phys::Shape::Polygon, phys::circleToPolygon);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Polygon, phys::Shape::Circle, phys::polygonToCircle);
    phys::CollisionDispatcher::registerCallback(phys::Shape::Polygon, phys::Shape::Polygon, phys::polygonToPolygon);
}

PhysicsSystem::~PhysicsSystem()
{
    delete mWorld;
}

void PhysicsSystem::begin(const float dt)
{
    for (auto entity : getActiveEntities())
    {
        auto trans = entity->getComponent<fsn::Transform>();
        auto phys = entity->getComponent<RigidBody>();

        phys->getBody()->setPosition(trans->getPosition()/PTU);
        phys->getBody()->setRotation(fsn::degToRad(trans->getRotation()));
    }

    mWorld->step();
}

void PhysicsSystem::processEntity(fsn::EntityRef* entity, const float dt)
{
    auto trans = entity->getComponent<fsn::Transform>();
    auto phys = entity->getComponent<RigidBody>();

    trans->setPosition(phys->getBody()->getPosition()*PTU);
    trans->setRotation(fsn::radToDeg(phys->getBody()->getRotation()));
}

void PhysicsSystem::end(const float dt)
{
}

void PhysicsSystem::onEntityAdded(fsn::EntityRef* entity)
{
    auto phys = entity->getComponent<RigidBody>();
    mWorld->addRigidBody(phys->getBody());
}

void PhysicsSystem::onEntityRemoved(fsn::EntityRef* entity)
{
    auto phys = entity->getComponent<RigidBody>();
    mWorld->removeRigidBody(phys->getBody());
}
