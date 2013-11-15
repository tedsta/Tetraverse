#include "PlaceableComponent.h"

#include <iostream>

std::map<std::string, sqext::SQIClass*> PlaceableComponent::Classes;
TypeBits PlaceableComponent::Type;

PlaceableComponent::PlaceableComponent(GridComponent* grid, const std::string& className) : mGrid(grid),
    mClassName(className), mInst(NULL)
{
    if (mClassName.size() > 0)
        mInst = new sqext::SQIClassInstance(Classes[mClassName]->New());
}

PlaceableComponent::~PlaceableComponent()
{
    delete mInst;
}

void PlaceableComponent::setClassName(const std::string& className)
{
    if (!mInst)
    {
        mClassName = className;
        if (mClassName.size() > 0)
            mInst = new sqext::SQIClassInstance(Classes[mClassName]->New());
        else
            mInst = NULL;
    }
}

void PlaceableComponent::registerClass(HSQUIRRELVM vm, const std::string& className)
{
    sqext::SQIClass* newClass = new sqext::SQIClass(className.c_str(), vm);
    newClass->bind("update");
    Classes[className] = newClass;
}
