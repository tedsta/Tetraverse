#include "Systems/EmitterSystem.h"

#include "Components/EmitterComponent.h"

EmitterSystem::EmitterSystem(fsn::IEventManager* eventManager, float lockStep) : System(eventManager, lockStep)
{
    mAspect.all<EmitterComponent>();
}

EmitterSystem::~EmitterSystem()
{
    //dtor
}

void EmitterSystem::begin(const float dt)
{
}

void EmitterSystem::processEntity(fsn::EntityRef* entity, const float dt)
{
}

void EmitterSystem::end(const float dt)
{
}
