#ifndef BACKGRIDCOMPONENT_H
#define BACKGRIDCOMPONENT_H

#include <Fission/Rendering/RenderComponent.h>

class GridComponent;

class BackGridComponent : public RenderComponent
{
    public:
        BackGridComponent(GridComponent* grid = NULL);
        virtual ~BackGridComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

        void render(sf::RenderTarget& target, sf::RenderStates states);
        void renderShadow(sf::RenderTarget& target, sf::RenderStates states){}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new BackGridComponent;}

        static void addTileSheet(int mat, sf::Texture* sheet)
        {
            if (mat >= static_cast<int>(TileSheets.size()))
                TileSheets.resize(mat+1);
            TileSheets[mat] = sheet;
        }

    private:
        int mGridID;

        static std::vector<sf::Texture*> TileSheets;
};

#endif // BACKGRIDCOMPONENT_H
