#include "PlaceableSystem.h"

#include <iostream>

#include "PlaceableComponent.h"

PlaceableSystem::PlaceableSystem(EventManager *eventManager, float lockStep) : System(eventManager, lockStep, PlaceableComponent::Type)
{
    //ctor
}

PlaceableSystem::~PlaceableSystem()
{
    //dtor
}

void PlaceableSystem::begin(const float dt)
{
}

void PlaceableSystem::processEntity(Entity *entity, const float dt)
{
    auto placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));

    try
    {
        if (placeable->mGrid && placeable->mInst)
            placeable->mInst->call("update", dt);
    }
    catch (Sqrat::Exception e)
    {
        std::cout << "Squirrel error: " << e.Message() << std::endl;
    }
}

void PlaceableSystem::end(const float dt)
{
}
