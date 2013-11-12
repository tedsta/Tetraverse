#include "SkeletonComponent.h"

TypeBits SkeletonComponent::Type;

SkeletonComponent::SkeletonComponent(ResourceManager* rc, std::string spData, std::string atlas)
{
    spSkeletonData* skData = (spSkeletonData*)rc->get(spData);
    spAnimationStateData* stateData = (spAnimationStateData*)spAnimationStateData_create(skData);
    mSkeleton = new spine::SkeletonDrawable(skData, stateData);
}

SkeletonComponent::~SkeletonComponent()
{
    //dtor
}
