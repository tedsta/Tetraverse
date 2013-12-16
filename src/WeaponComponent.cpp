#include "WeaponComponent.h"

#include <Fission/Core/Math.h>

TypeBits WeaponComponent::Type;

WeaponComponent::WeaponComponent()
{
    //ctor
}

WeaponComponent::~WeaponComponent()
{
    //dtor
}

void WeaponComponent::serialize(sf::Packet &packet)
{
}

void WeaponComponent::deserialize(sf::Packet &packet)
{
}

void WeaponComponent::postDeserialize()
{
}

bool WeaponComponent::fire(float rot)
{
    sf::Vector2f dir(cos(degToRad(rot)), sin(degToRad(rot)));

    return true;
}
