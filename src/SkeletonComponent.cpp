#include "SkeletonComponent.h"

#include <Fission/Core/ResourceManager.h>

TypeBits SkeletonComponent::Type;

SkeletonComponent::SkeletonComponent(std::string skDataFile, std::string atlasFile) : mSkDataFile(skDataFile), mAtlasFile(atlasFile)
{
    if (skDataFile.size() == 0 || atlasFile.size() == 0)
        return;

    spAtlas* atlas = reinterpret_cast<spAtlas*>(ResourceManager::get()->get(mAtlasFile));
    if (!atlas)
    {
        atlas = spAtlas_readAtlasFile(mAtlasFile.c_str());
        ResourceManager::get()->add(mAtlasFile, atlas);
    }

    spSkeletonData* skData = reinterpret_cast<spSkeletonData*>(ResourceManager::get()->get(mSkDataFile));
    if (!skData)
    {
        spSkeletonJson* json = spSkeletonJson_create(atlas);
        skData = spSkeletonJson_readSkeletonDataFile(json, mSkDataFile.c_str());
        spSkeletonJson_dispose(json);
        ResourceManager::get()->add(mSkDataFile, skData);
    }

    spAnimationStateData* stateData = reinterpret_cast<spAnimationStateData*>(ResourceManager::get()->get(mSkDataFile+"_anim"));
    if (!stateData)
    {
        stateData = spAnimationStateData_create(skData);
        ResourceManager::get()->add(mSkDataFile+"_anim", stateData);
    }

    mSkeleton = new spine::SkeletonDrawable(skData, stateData);
}

SkeletonComponent::~SkeletonComponent()
{
    delete mSkeleton;
}

void SkeletonComponent::serialize(sf::Packet &packet)
{
    RenderComponent::serialize(packet);

    packet << mSkDataFile;
    packet << mAtlasFile;
}

void SkeletonComponent::deserialize(sf::Packet &packet)
{
    RenderComponent::deserialize(packet);

    packet >> mSkDataFile;
    packet >> mAtlasFile;

    spAtlas* atlas = reinterpret_cast<spAtlas*>(ResourceManager::get()->get(mAtlasFile));
    if (!atlas)
    {
        atlas = spAtlas_readAtlasFile(mAtlasFile.c_str());
        ResourceManager::get()->add(mAtlasFile, atlas);
    }

    spSkeletonData* skData = reinterpret_cast<spSkeletonData*>(ResourceManager::get()->get(mSkDataFile));
    if (!skData)
    {
        spSkeletonJson* json = spSkeletonJson_create(atlas);
        skData = spSkeletonJson_readSkeletonDataFile(json, mSkDataFile.c_str());
        spSkeletonJson_dispose(json);
        ResourceManager::get()->add(mSkDataFile, skData);
    }

    spAnimationStateData* stateData = reinterpret_cast<spAnimationStateData*>(ResourceManager::get()->get(mSkDataFile+"_anim"));
    if (!stateData)
    {
        stateData = spAnimationStateData_create(skData);
        ResourceManager::get()->add(mSkDataFile+"_anim", stateData);
    }

    mSkeleton = new spine::SkeletonDrawable(skData, stateData);
}

void SkeletonComponent::postDeserialize()
{
}

void SkeletonComponent::render(sf::RenderTarget& target, sf::RenderStates states)
{
    mSkeleton->draw(target, states);
}

void SkeletonComponent::renderShadow(sf::RenderTarget& target, sf::RenderStates states)
{
    //mSkeleton->draw(target, states);
}

spAnimation* SkeletonComponent::findAnimation(const std::string& name)
{
    return spSkeletonData_findAnimation(mSkeleton->skeleton->data, name.c_str());
}

void SkeletonComponent::setAnimation(spAnimation* animation, bool loop)
{
    spAnimationState_setAnimation(mSkeleton->state, 0, animation, loop);
}

void SkeletonComponent::addAnimation(spAnimation* animation, bool loop, float delay)
{
    spAnimationState_addAnimation(mSkeleton->state, 0, animation, loop, delay);
}

void SkeletonComponent::clearAnimation()
{
    spAnimationState_clearTrack(mSkeleton->state, 0);
}

void SkeletonComponent::update(float dt)
{
    mSkeleton->update(dt);
}

std::string SkeletonComponent::getCurrentAnimation()
{
    spTrackEntry* entry = spAnimationState_getCurrent(mSkeleton->state, 0);
    if (entry && entry->animation)
        return entry->animation->name;
    return "";
}

