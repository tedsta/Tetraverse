#include "LightComponent.h"

TypeBits LightComponent::Type;

LightComponent::LightComponent(float _radius) : radius(_radius)
{
    // Set up the light mask
    lightMask.create(radius*2/4, radius*2/4);
    sf::View view = lightMask.getDefaultView();
    view.zoom(4.f);
    lightMask.setView(view);
    lightMaskSprite.setTexture(lightMask.getTexture());
    lightMaskSprite.setOrigin(lightMask.getSize().x/2, lightMask.getSize().y/2);
    lightMaskSprite.setScale(1.f, 1.f);

    lightMaskClear.setSize(sf::Vector2f(lightMask.getSize().x, lightMask.getSize().y));
    lightMaskClear.setFillColor(sf::Color::Transparent);
    lightMaskClear.setOrigin(lightMask.getSize().x/2, lightMask.getSize().y/2);

    // Set up the shadow map
    shadowMap.create(lightMask.getSize().x, lightMask.getSize().y);
    shadowMap.setSmooth(false);
    shadowMap.setRepeated(true);
    shadowMapSprite.setTexture(shadowMap.getTexture());
    shadowMapSprite.setScale(4.f, -4.f);
}

LightComponent::~LightComponent()
{
    //dtor
}
