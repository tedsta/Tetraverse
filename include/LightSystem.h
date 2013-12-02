#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <Fission/Core/System.h>

class RenderSystem;

class LightSystem : public System
{
    public:
        LightSystem(EventManager *eventManager, RenderSystem* _rndSys, float lockStep);
        virtual ~LightSystem();

    protected:
        void begin(const float dt);

        void processEntity(Entity *entity, const float dt);

        void end(const float dt);

    private:
        RenderSystem* rndSys;
        sf::RenderTexture lightMask; // The final light mask to be rendered
        sf::Sprite lightMaskSprite;

        sf::RenderTexture hBlurMask;
        sf::Sprite hBlurMaskSprite;
        sf::RenderTexture vBlurMask;
        sf::Sprite vBlurMaskSprite;

        sf::Shader shadowMapGen;
        sf::Shader shadowRnd;
        sf::Shader hBlur;
        sf::Shader vBlur;
};

#endif // LIGHTSYSTEM_H
