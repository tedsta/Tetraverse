#include "SkeletonComponent.h"

TypeBits SkeletonComponent::Type;

SkeletonComponent::SkeletonComponent(ResourceManager* rc, std::string skDataFile, std::string atlasFile)
{
    /*spAtlas* atlas = reinterpret_cast<spAtlas*>(rc->get(atlasFile));
    if (!atlas)
    {
        atlas = spAtlas_readAtlasFile(atlasFile.c_str());
        rc->add(atlasFile, atlas);
    }

    spSkeletonData* skData = reinterpret_cast<spSkeletonData*>(rc->get(skDataFile));
    if (!skData)
    {
        skData = spSkeletonJson_readSkeletonDataFile();
        skData->
    }

    spAnimationStateData* stateData = reinterpret_cast<spAnimationStateData*>(spAnimationStateData_create(skData));
    mSkeleton = new spine::SkeletonDrawable(skData, stateData);*/
}

SkeletonComponent::~SkeletonComponent()
{
    //dtor
}
