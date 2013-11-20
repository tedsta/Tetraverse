#include "PlayerComponent.h"

#include <Fission/Input/Defs.h>

TypeBits PlayerComponent::Type;

PlayerComponent::PlayerComponent() : mLeftHand(0), mRightHand(1),
    mLeftHandState(BtnState::UP), mRightHandState(BtnState::UP), mStupidMode(false)
{
    //ctor
}

PlayerComponent::~PlayerComponent()
{
    //dtor
}

void PlayerComponent::serialize(sf::Packet &packet)
{
}

void PlayerComponent::deserialize(sf::Packet &packet)
{
}
