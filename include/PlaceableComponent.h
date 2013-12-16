#ifndef PLACEABLECOMPONENT_H
#define PLACEABLECOMPONENT_H

#include <string>
#include <map>
#include <Sqrat/sqext.h>
#include <Fission/Core/Component.h>

class Entity;
class GridComponent;

class PlaceableComponent : public Component
{
    friend class PlaceableSystem;

    public:
        PlaceableComponent(Entity* entity = NULL, Entity* grid = NULL, const std::string& className = "", int gridX = 1, int gridY = 1, int width = 1, int height = 1);
        virtual ~PlaceableComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);
        void postDeserialize();

        static void registerClass(HSQUIRRELVM vm, const std::string& className);

        void interact();

        // Setters
        void setGrid(Entity* grid);
        void setGridX(int x){mGridX=x;}
        void setGridY(int y){mGridY=y;}
        void setGridPos(int x, int y);

        // Getters
        int getGridX(){return mGridX;}
        int getGridY(){return mGridY;}
        int getWidth(){return mWidth;}
        int getHeight(){return mHeight;}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new PlaceableComponent;}

    private:
        static std::map<std::string, sqext::SQIClass*> Classes;

        int mEntity;
        int mGrid;
        std::string mClassName;
        sqext::SQIClassInstance* mInst; // The instance of the squirrel class that does the logic

        int mGridX;
        int mGridY;
        int mWidth;
        int mHeight;
};

#endif // PLACEABLECOMPONENT_H
