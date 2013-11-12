#ifndef SKELETONCOMPONENT_H
#define SKELETONCOMPONENT_H

#include <spine/spine-sfml.h>
#include <Fission/Core/ResourceManager.h>
#include <Fission/Core/Component.h>

class SkeletonComponent : public Component
{
    public:
        SkeletonComponent(ResourceManager* rc = NULL, std::string spData = "", std::string atlas = "");
        virtual ~SkeletonComponent();

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new SkeletonComponent;}

    private:
        spine::SkeletonDrawable* mSkeleton;
};

#endif // SKELETONCOMPONENT_H
