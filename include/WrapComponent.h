#ifndef WRAPCOMPONENT_H
#define WRAPCOMPONENT_H

#include <Fission/Rendering/RenderComponent.h>

class GridComponent;

class WrapComponent : public RenderComponent
{
    public:
        WrapComponent(RenderComponent* rnd = NULL);
        virtual ~WrapComponent();

        void render(sf::RenderTarget& target, sf::RenderStates states);

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);
        void postDeserialize();

        void setGrid(GridComponent* grid){mGrid=grid;}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new WrapComponent;}

    private:
        RenderComponent* mRnd;
        GridComponent* mGrid;

        // Used purely for deserialization
        int mRndID;
};

#endif // WRAPCOMPONENT_H
