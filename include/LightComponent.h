#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <Fission/Core/Component.h>

class LightComponent : public Component
{
    friend class LightSystem;

    public:
        enum LightType
        {
            POINT
        };

        LightComponent(float _radius = 0.f);
        LightComponent(const LightComponent& other){}
        virtual ~LightComponent();

        void setActive(bool a){active=a;}

        bool getActive(){return active;}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new LightComponent;}

    private:
        // The light mask
        sf::RenderTexture lightMask;
        sf::Sprite lightMaskSprite;
        sf::RectangleShape lightMaskClear; // Workaround for SFML bug

        sf::RenderTexture shadowMap;
        sf::Sprite shadowMapSprite;

        float radius;
        sf::Color color;
        bool active;
};

#endif // LIGHTCOMPONENT_H
