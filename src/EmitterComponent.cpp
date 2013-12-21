#include "EmitterComponent.h"

TypeBits EmitterComponent::Type;

EmitterComponent::EmitterComponent()
{
    //ctor
}

EmitterComponent::~EmitterComponent()
{
    //dtor
}

void EmitterComponent::serialize(sf::Packet &packet)
{
}

void EmitterComponent::deserialize(sf::Packet &packet)
{
}

void EmitterComponent::postDeserialize()
{
}

void EmitterComponent::render(sf::RenderTarget& target, sf::RenderStates states)
{
}
