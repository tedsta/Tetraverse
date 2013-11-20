#include "PlaceableComponent.h"

#include <iostream>

#include <Fission/Core/Entity.h>
#include "GridComponent.h"

std::map<std::string, sqext::SQIClass*> PlaceableComponent::Classes;
TypeBits PlaceableComponent::Type;

PlaceableComponent::PlaceableComponent(Entity* grid, const std::string& className, int gridX, int gridY, int width, int height) :
    mGrid(grid), mClassName(className),
    mInst(NULL), mGridX(gridX), mGridY(gridY), mWidth(width), mHeight(height)
{
    if (grid && mClassName.size() > 0)
        mInst = new sqext::SQIClassInstance(Classes[mClassName]->New(grid, gridX, gridY));
}

PlaceableComponent::~PlaceableComponent()
{
    delete mInst;
}

void PlaceableComponent::interact()
{
    try
    {
        if (mGrid && mInst)
            mInst->call("interact");
    }
    catch (Sqrat::Exception e)
    {
        std::cout << "Squirrel error: " << e.Message() << std::endl;
    }
}

void PlaceableComponent::setGrid(Entity* grid)
{
    mGrid = grid;
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
