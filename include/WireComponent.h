#ifndef WireCOMPONENT_H
#define WireCOMPONENT_H

#include <Fission/Rendering/RenderComponent.h>

#define WireMax 255
class WireComponent : public RenderComponent
{
    public:
        WireComponent();

        void render(sf::RenderTarget& target, sf::RenderStates states);
        void renderShadow(sf::RenderTarget& target, sf::RenderStates states);

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new WireComponent();}
        int load(int value);
        void connect(WireComponent* wire){};



    protected:
        void update();
        WireComponent* red;
        WireComponent* black;
        WireComponent* wire;
        int power;
        int drain;
        int ground;
};

#endif // ELECTRICCOMPONENT_H
