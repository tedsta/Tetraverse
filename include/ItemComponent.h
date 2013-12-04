#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H

#include <queue>
#include <Sqrat/sqrat.h>
#include <SFML/Graphics/Texture.hpp>
#include <Fission/Core/Component.h>

class Scene;
class Entity;
class GridComponent;
class PlayerComponent;

class Item
{
    public:
        Item(std::string name, const std::string& texturePath, bool consumable, int useState, int maxStack, int coordCount, HSQUIRRELVM vm, std::string useFunc);

        bool use(Entity* grid, std::queue<sf::Vector2f>& coords);
        Entity* spawn(Scene* scene, sf::Vector2f pos);

        // Getters
        const std::string& getTexturePath(){return mTexturePath;}
        bool getConsumable(){return mConsumable;}
        int getUseState(){return mUseState;}
        int getMaxStack(){return mMaxStack;}
        int getCoordCount(){return mCoordCount;}

        static std::vector<Item*> Items;

    private:
        std::string mName;
        std::string mTexturePath;
        bool mConsumable;
        int mUseState;
        int mMaxStack;
        int mCoordCount; // How many coordinates this item needs in order to be used
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
