#include "ElectricSystem.h"
#include "ElectricComponent.h"
#include <iostream>

ElectricSystem::ElectricSystem(EventManager *eventManager) : System(eventManager, GridComponent::Type)
{
    //ctor
}

void ElectricSystem::begin(const float dt)
{
}

void ElectricSystem::processEntity(Entity *entity, const float dt)
{
    ElectricComponent * wire = static_cast<ElectricComponent*>(entity->getComponent(ElectricComponent::Type));
    wire->update();
}

void ElectricSystem::end(const float dt)
{
}
