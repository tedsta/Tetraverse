#ifndef SKELETONCOMPONENT_H
#define SKELETONCOMPONENT_H

#include <spine/spine-sfml.h>
#include <Fission/Rendering/RenderComponent.h>

class SkeletonComponent : public RenderComponent
{
    public:
        SkeletonComponent(std::string skDataFile = "", std::string atlasFile = "");
        virtual ~SkeletonComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

        void render(sf::RenderTarget& target, sf::RenderStates states);
        void renderShadow(sf::RenderTarget& target, sf::RenderStates states);

        spAnimation* findAnimation(const std::string& name);
        void setAnimation(spAnimation* animation, bool loop);
        void addAnimation(spAnimation* animation, bool loop, float delay);
        void clearAnimation();
        void update(float dt);
        std::string getCurrentAnimation();

        spine::SkeletonDrawable* getSkeleton(){return mSkeleton;}
        sf::FloatRect getBounds() const {return sf::FloatRect(0, 0, 0, 0);}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new SkeletonComponent;}

    private:
        spine::SkeletonDrawable* mSkeleton;
};

#endif // SKELETONCOMPONENT_H
