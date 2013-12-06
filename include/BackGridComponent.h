#ifndef BACKGRIDCOMPONENT_H
#define BACKGRIDCOMPONENT_H

#include <Fission/Rendering/RenderComponent.h>

class GridComponent;

class BackGridComponent : public RenderComponent
{
    public:
        BackGridComponent(GridComponent* grid = NULL);
        virtual ~BackGridComponent();

        void render(sf::RenderTarget& target, sf::RenderStates states);
        void renderShadow(sf::RenderTarget& target, sf::RenderStates states){}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new BackGridComponent;}

    private:
        GridComponent* mGrid;
};

#endif // BACKGRIDCOMPONENT_H