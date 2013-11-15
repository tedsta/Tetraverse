#ifndef PLACEABLECOMPONENT_H
#define PLACEABLECOMPONENT_H

#include <string>
#include <map>
#include <Sqrat/sqext.h>
#include <Fission/Core/Component.h>

class GridComponent;

class PlaceableComponent : public Component
{
    friend class PlaceableSystem;

    public:
        PlaceableComponent(GridComponent* grid = NULL, const std::string& className = "");
        virtual ~PlaceableComponent();

        static void registerClass(HSQUIRRELVM vm, const std::string& className);

        // Setters
        void setGrid(GridComponent* grid){mGrid=grid;}
        void setClassName(const std::string& className);

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new PlaceableComponent;}

    private:
        static std::map<std::string, sqext::SQIClass*> Classes;

        GridComponent* mGrid;
        std::string mClassName;
        sqext::SQIClassInstance* mInst; // The instance of the squirrel class that does the logic
};

#endif // PLACEABLECOMPONENT_H
