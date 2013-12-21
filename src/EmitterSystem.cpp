#include "EmitterSystem.h"

#include "EmitterComponent.h"

EmitterSystem::EmitterSystem(EventManager *eventManager, float lockStep) : System(eventManager, lockStep, EmitterComponent::Type)
{
    //ctor
}

EmitterSystem::~EmitterSystem()
{
    //dtor
}

void EmitterSystem::begin(const float dt)
{
}

void EmitterSystem::processEntity(Entity *entity, const float dt)
{
}

void EmitterSystem::end(const float dt)
{
}
