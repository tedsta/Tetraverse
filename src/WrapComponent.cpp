#include "WrapComponent.h"

#include <Fission/Rendering/TransformComponent.h>

#include "GridComponent.h"

TypeBits WrapComponent::Type;

WrapComponent::WrapComponent(RenderComponent* rnd) : mRnd(rnd)
{
    //ctor
}

WrapComponent::~WrapComponent()
{
    //dtor
}

void WrapComponent::render(sf::RenderTarget& target, sf::RenderStates states)
{
    float tsize = static_cast<float>(TILE_SIZE);

	sf::Vector2f center = mGrid->getTransform()->getTransform().transformPoint(sf::Vector2f());
	center -= target.getView().getCenter();
	center += sf::Vector2f(target.getView().getSize().x, target.getView().getSize().y)/2.f;
	center.x *= -1;
	center.y *= -1;

    float currentOffsetX = -mGrid->getSizeX()*tsize;

	while (center.x+currentOffsetX > 0)
    {
        sf::RenderStates newStates = states;
        newStates.transform.translate(-mGrid->getSizeX()*tsize, 0.f);
        mRnd->render(target, newStates);

        currentOffsetX -= mGrid->getSizeX()*tsize;
    }
}

void WrapComponent::serialize(sf::Packet &packet)
{
    RenderComponent::serialize(packet);
}

void WrapComponent::deserialize(sf::Packet &packet)
{
    RenderComponent::deserialize(packet);
}

void WrapComponent::postDeserialize()
{
}
