#include "ItemComponent.h"

#include <iostream>

#include <Fission/Core/Scene.h>
#include <Fission/Core/Entity.h>
#include <Fission/Rendering/TransformComponent.h>
#include <Fission/Rendering/SpriteComponent.h>

#include "PhysicsComponent.h"

std::vector<Item*> Item::Items;

Item::Item(std::string name, const std::string& texturePath, bool consumable, int useState, int maxStack, int coordCount, HSQUIRRELVM vm, std::string useFunc) :
    mName(name), mTexturePath(texturePath), mConsumable(consumable), mUseState(useState), mMaxStack(maxStack), mCoordCount(coordCount)
{
    mUseFunc = Sqrat::RootTable(vm).GetFunction(useFunc.c_str());
}

bool Item::use(Entity* grid, std::queue<sf::Vector2f>& coords)
{
    bool used = false;
    try
    {
        Sqrat::Array sqCoords(mUseFunc.GetVM());
        while (!coords.empty())
        {
            sqCoords.Append(coords.front());
            coords.pop();
        }
        used = mUseFunc.Evaluate<bool>(grid, sqCoords);
    }
    catch (Sqrat::Exception e)
    {
        std::cout << "Squirrel error: " << e.Message() << std::endl;
    }
    return used;
}

Entity* Item::spawn(Scene* scene, sf::Vector2f pos)
{
    Entity* e = scene->createEntity();
    e->addComponent(new TransformComponent(pos));
    e->addComponent(new SpriteComponent(mTexturePath));
    e->addComponent(new PhysicsComponent);
    e->addComponent(new ItemComponent);
    return e;
}

// ************************************************************************************************

TypeBits ItemComponent::Type;

ItemComponent::ItemComponent()
{
    //ctor
}

ItemComponent::~ItemComponent()
{
    //dtor
}

void ItemComponent::serialize(sf::Packet &packet)
{
}

void ItemComponent::deserialize(sf::Packet &packet)
{
}
