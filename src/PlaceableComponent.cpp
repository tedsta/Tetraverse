#include "PlaceableComponent.h"

#include <iostream>

#include <Fission/Core/Entity.h>
#include "GridComponent.h"

std::map<std::string, sqext::SQIClass*> PlaceableComponent::Classes;
TypeBits PlaceableComponent::Type;

PlaceableComponent::PlaceableComponent(Entity* entity, Entity* grid, const std::string& className, int gridX, int gridY, int width, int height) :
    mClassName(className), mInst(NULL), mGridX(gridX), mGridY(gridY), mWidth(width), mHeight(height)
{
    if (entity)
        mEntity = entity->getID();

    if (grid)
        mGrid = grid->getID();

    if (entity && grid && !mInst && mClassName.size() > 0)
        mInst = new sqext::SQIClassInstance(Classes[mClassName]->New(entity, grid, gridX, gridY));
}

PlaceableComponent::~PlaceableComponent()
{
    delete mInst;
}

void PlaceableComponent::serialize(sf::Packet &packet)
{
    packet << mEntity;
    packet << mGrid;
    packet << mClassName;
    packet << mGridX << mGridY << mWidth << mHeight;
}

void PlaceableComponent::deserialize(sf::Packet &packet)
{
    packet >> mEntity;
    packet >> mGrid;
    packet >> mClassName;
    packet >> mGridX >> mGridY >> mWidth >> mHeight;
}

void PlaceableComponent::postDeserialize()
{
    Entity* entity = Entity::get(mEntity);
    Entity* grid = Entity::get(mGrid);
    if (entity && grid && mClassName.size() > 0)
        mInst = new sqext::SQIClassInstance(Classes[mClassName]->New(entity, grid, mGridX, mGridY));
}

void PlaceableComponent::interact()
{
    Entity* entity = Entity::get(mEntity);
    Entity* grid = Entity::get(mGrid);

    try
    {
        if (grid && mInst)
            mInst->call("interact");
    }
    catch (Sqrat::Exception e)
    {
        std::cout << "Squirrel error: " << e.Message() << std::endl;
    }
}

void PlaceableComponent::setGrid(Entity* grid)
{
    mGrid = grid->getID();
    mInst->call("setGrid", grid);
}

void PlaceableComponent::setGridPos(int x, int y)
{
    mGridX = x;
    mGridY = y;
    mInst->call("setGridPos", x, y);
}

void PlaceableComponent::registerClass(HSQUIRRELVM vm, const std::string& className)
{
    sqext::SQIClass* newClass = new sqext::SQIClass(className.c_str(), vm);
    newClass->bind("update");
    Classes[className] = newClass;
}
