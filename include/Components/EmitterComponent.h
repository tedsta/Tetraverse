#ifndef EMITTERCOMPONENT_H
#define EMITTERCOMPONENT_H

#include <Fission/Rendering/RenderComponent.h>

class EmitterComponent : public fsn::RenderComponent
{
    FISSION_COMPONENT

    public:
        EmitterComponent();
        virtual ~EmitterComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

    private:
};

#endif // EMITTERCOMPONENT_H
