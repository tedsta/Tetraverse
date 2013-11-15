#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H

#include <Sqrat/sqrat.h>
#include <SFML/Graphics/Texture.hpp>
#include <Fission/Core/Component.h>

class Scene;
class Entity;
class GridComponent;

class Item
{
    public:
        Item(std::string name, const std::string& texturePath, bool consumable, int maxStack, HSQUIRRELVM vm, std::string useFunc);

        bool use(GridComponent* grid, int x, int y);
        Entity* spawn(Scene* scene, sf::Vector2f pos);

        // Getters
        const std::string& getTexturePath(){return mTexturePath;}
        bool getConsumable(){return mConsumable;}
        int getMaxStack(){return mMaxStack;}

        static std::vector<Item*> Items;

    private:
        std::string mName;
        std::string mTexturePath;
        bool mConsumable;
        int mMaxStack;
        Sqrat::Function mUseFunc;
};

class ItemComponent : public Component
{
    public:
        ItemComponent();
        virtual ~ItemComponent();

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new ItemComponent;}

    private:
};

#endif // ITEMCOMPONENT_H
