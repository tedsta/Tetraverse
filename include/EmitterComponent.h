#ifndef EMITTERCOMPONENT_H
#define EMITTERCOMPONENT_H

#include <Fission/Rendering/RenderComponent.h>

class EmitterComponent : public RenderComponent
{
    public:
        EmitterComponent();
        virtual ~EmitterComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);
        void postDeserialize();

        void render(sf::RenderTarget& target, sf::RenderStates states);

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new EmitterComponent;}

    private:
};

#endif // EMITTERCOMPONENT_H
