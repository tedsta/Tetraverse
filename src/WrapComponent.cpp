#include "WrapComponent.h"

#include <Fission/Rendering/TransformComponent.h>

#include "GridComponent.h"

TypeBits WrapComponent::Type;

WrapComponent::WrapComponent(RenderComponent* rnd) : mRnd(rnd), mGrid(NULL)
{
}

WrapComponent::~WrapComponent()
{
    //dtor
}

void WrapComponent::render(sf::RenderTarget& target, sf::RenderStates states)
{
    setLayer(mRnd->getLayer());

    if (!mGrid || !mGrid->getWrapX())
        return;

    float tsize = static_cast<float>(TILE_SIZE);

	sf::Vector2f pos = states.transform.transformPoint(sf::Vector2f());

    float currentOffsetX = -mGrid->getSizeX()*tsize;
    sf::RenderStates leftStates = states;
	while (pos.x+currentOffsetX > target.getView().getCenter().x-target.getView().getSize().x/2)
    {
        leftStates.transform.translate(-mGrid->getSizeX()*tsize, 0.f);
        mRnd->render(target, leftStates);

        currentOffsetX -= mGrid->getSizeX()*tsize;
    }

    currentOffsetX = mGrid->getSizeX()*tsize;
    sf::RenderStates rightStates = states;
	while (pos.x+currentOffsetX < target.getView().getCenter().x+target.getView().getSize().x/2)
    {
        rightStates.transform.translate(mGrid->getSizeX()*tsize, 0.f);
        mRnd->render(target, rightStates);

        currentOffsetX += mGrid->getSizeX()*tsize;
    }
}

void WrapComponent::serialize(sf::Packet &packet)
{
    RenderComponent::serialize(packet);

    packet << mRnd->getID();
}

void WrapComponent::deserialize(sf::Packet &packet)
{
    RenderComponent::deserialize(packet);

    packet >> mRndID;
}

void WrapComponent::postDeserialize()
{
    mRnd = reinterpret_cast<RenderComponent*>(Component::get(mRndID));
}
