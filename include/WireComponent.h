#ifndef WireCOMPONENT_H
#define WireCOMPONENT_H

#include "Fission/Rendering/RenderComponent.h"

#define WireMax 255
class WireComponent : public WireComponent
{
    public:
        WireComponent();

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        //static Component* factory() {return new ElectricComponent();}
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
