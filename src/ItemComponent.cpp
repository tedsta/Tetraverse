#include "ItemComponent.h"

#include <iostream>

#include "Fission/Core/Scene.h"
#include "Fission/Core/Entity.h"
#include "Fission/Rendering/TransformComponent.h"
#include "Fission/Rendering/SpriteComponent.h"
#include "PhysicsComponent.h"

std::vector<Item*> Item::Items;

Item::Item(std::string name, sf::Texture* texture, bool consumable, int maxStack, HSQUIRRELVM vm, std::string useFunc) :
    mName(name), mTexture(texture), mConsumable(consumable), mMaxStack(maxStack)
{
    mUseFunc = Sqrat::RootTable(vm).GetFunction(useFunc.c_str());
}

void Item::use(GridComponent* grid, int x, int y)
{
    try
    {
        mUseFunc.Execute(grid, x, y);
    }
    catch (Sqrat::Exception e)
    {
        std::cout << "Squirrel error: " << e.Message() << std::endl;
    }
}

Entity* Item::spawn(Scene* scene, sf::Vector2f pos)
{
    Entity* e = scene->createEntity();
    e->addComponent(new TransformComponent(pos));
    e->addComponent(new SpriteComponent(mTexture));
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
