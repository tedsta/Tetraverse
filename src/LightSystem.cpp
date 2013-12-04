#include "LightSystem.h"

#include <Fission/Rendering/RenderSystem.h>
#include <Fission/Rendering/RenderComponent.h>
#include <Fission/Rendering/TransformComponent.h>

#include <iostream>

#include "LightComponent.h"

LightSystem::LightSystem(EventManager *eventManager, RenderSystem* _rndSys, float lockStep) :
    System(eventManager, lockStep, TransformComponent::Type|LightComponent::Type), rndSys(_rndSys)
{
    lightMask.create(rndSys->getWindow().getSize().x, rndSys->getWindow().getSize().y);
    lightMaskSprite.setTexture(lightMask.getTexture());

    hBlurMask.create(lightMask.getSize().x, lightMask.getSize().y);
    hBlurMaskSprite.setTexture(hBlurMask.getTexture());
    vBlurMask.create(lightMask.getSize().x, lightMask.getSize().y);
    vBlurMaskSprite.setTexture(vBlurMask.getTexture());

    rndSys->addAdditionalSprite(&vBlurMaskSprite);

    shadowMapGen.loadFromFile("Content/Shaders/shadowMapGen.frag", sf::Shader::Fragment);
    shadowRnd.loadFromFile("Content/Shaders/shadowRnd.frag", sf::Shader::Fragment);
    hBlur.loadFromFile("Content/Shaders/hblur.vert", "Content/Shaders/blur.frag");
    vBlur.loadFromFile("Content/Shaders/vblur.vert", "Content/Shaders/blur.frag");
}

LightSystem::~LightSystem()
{
    //dtor
}

void LightSystem::begin(const float dt)
{
    lightMask.clear(sf::Color::Black);
    lightMask.setView(rndSys->getView());
}

void LightSystem::processEntity(Entity *entity, const float dt)
{
    auto transform = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    auto light = reinterpret_cast<LightComponent*>(entity->getComponent(LightComponent::Type));

    if (!light->getActive())
        return;

    sf::FloatRect fRect = transform->getTransform().transformRect(light->lightMaskSprite.getGlobalBounds());
    sf::IntRect lightRect(fRect.left, fRect.top, fRect.width, fRect.height);
    if (!lightMask.getViewport(lightMask.getView()).intersects(lightRect))
    {
        //std::cout << "aww yeah\n";
        //return;
    }

    light->lightMask.clear(sf::Color::Transparent);
    for (auto rEntity : rndSys->getActiveEntities())
    {
        auto rTransform = reinterpret_cast<TransformComponent*>(rEntity->getComponent(TransformComponent::Type));
        auto renderCmpnts = rEntity->getComponents(rndSys->getOptBits());

        sf::RenderStates states = sf::RenderStates::Default;
        states.transform = transform->getInverseTransform();
        states.transform.combine(rTransform->getTransform());
        states.transform.translate(light->lightMask.getSize().x/2, light->lightMask.getSize().y/2);

        for (auto cmpnt : renderCmpnts)
        {
            RenderComponent* rc = reinterpret_cast<RenderComponent*>(cmpnt);
            if (rc->getLit())
            {
                rc->renderShadow(light->lightMask, states);
            }
        }
    }

    light->lightMask.draw(light->lightMaskClear);
    light->lightMask.display();

    light->shadowMap.clear(sf::Color::Black);
    light->lightMaskSprite.setOrigin(0, 0);
    shadowMapGen.setParameter("u_texture", sf::Shader::CurrentTexture);
    shadowMapGen.setParameter("resolution", light->lightMask.getSize().x, light->lightMask.getSize().y);
    sf::RenderStates states = sf::RenderStates::Default;
    states.shader = &shadowMapGen;
    light->shadowMap.draw(light->lightMaskSprite, states);
    light->shadowMap.display();

    states = sf::RenderStates::Default;
    states.blendMode = sf::BlendMode::BlendMultiply;
    states.transform = transform->getTransform();
    shadowRnd.setParameter("u_texture", sf::Shader::CurrentTexture);
    shadowRnd.setParameter("resolution", light->lightMask.getSize().x, light->lightMask.getSize().y);
    states.shader = &shadowRnd;
    light->shadowMapSprite.setOrigin(light->lightMask.getSize().x/2, light->lightMask.getSize().y/2);
    lightMask.draw(light->shadowMapSprite, states);
}

void LightSystem::end(const float dt)
{
    lightMask.display();

    hBlurMask.clear(sf::Color::Transparent);
    sf::RenderStates states = sf::RenderStates::Default;
    hBlur.setParameter("s_texture", sf::Shader::CurrentTexture);
    states.shader = &hBlur;
    states.blendMode = sf::BlendMode::BlendNone;
    hBlurMask.draw(lightMaskSprite, states);
    hBlurMask.display();

    vBlurMask.clear(sf::Color::Transparent);
    states = sf::RenderStates::Default;
    vBlur.setParameter("s_texture", sf::Shader::CurrentTexture);
    states.shader = &vBlur;
    states.blendMode = sf::BlendMode::BlendNone;
    vBlurMask.draw(hBlurMaskSprite, states);
    vBlurMask.display();
}
