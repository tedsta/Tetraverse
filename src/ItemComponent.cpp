#include "ItemComponent.h"

#include "Fission/Core/Scene.h"
#include "Fission/Core/Entity.h"
#include "Fission/Rendering/TransformComponent.h"
#include "Fission/Rendering/SpriteComponent.h"
#include "PhysicsComponent.h"

Item::Item(std::string name, sf::Texture* texture, HSQUIRRELVM vm, std::string useFunc) :
    mName(name), mTexture(texture)
{
    mUseFunc = Sqrat::RootTable(vm).GetFunction(useFunc.c_str());
}

void Item::use()
{
    mUseFunc.Execute();
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
