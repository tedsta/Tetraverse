#ifndef ELECTRICCOMPONENT_H
#define ELECTRICCOMPONENT_H

#include "Fission/Rendering/RenderComponent.h"

#define WireMax 255
class ElectricComponent : public RenderComponent
{
    friend class ElectricSystem;

    public:
        ElectricComponent();

        void render(sf::RenderTarget& target, sf::RenderStates states);

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new ElectricComponent();}
        int load(int value);
        void connect(ElectricComponent* wire){};



    protected:
        void update();
        ElectricComponent* wire;
        int power;
        int drain;
        int ground;
};

#endif // ELECTRICCOMPONENT_H
