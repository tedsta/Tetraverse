#ifndef PLAYERCOMPONENT_H
#define PLAYERCOMPONENT_H

#include <stack>
#include <SFML/System/Vector2.hpp>

#include <Fission/Core/Component.h>

class PlayerComponent : public Component
{
    friend class PlayerSystem;

    public:
        PlayerComponent();
        virtual ~PlayerComponent();

        int getLeftCoordsCount(){return mLeftCoords.size();}
        void pushLeftCoord(sf::Vector2f coord){mLeftCoords.push(coord);}
        const sf::Vector2f& topLeftCoord(){return mLeftCoords.top();}
        void popLeftCoord(){mLeftCoords.pop();}

        int getRightCoordsCount(){return mRightCoords.size();}
        void pushRightCoord(sf::Vector2f coord){mRightCoords.push(coord);}
        const sf::Vector2f& topRightCoord(){return mRightCoords.top();}
        void popRightCoord(){mRightCoords.pop();}

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new PlayerComponent;}

    private:
        int mNetID;
        float mAnimTime;
        sf::Vector2f mCam;
        bool mStupidMode;
        int mLeftHand;
        int mRightHand;
        std::stack<sf::Vector2f> mLeftCoords;
        std::stack<sf::Vector2f> mRightCoords;
};

#endif // PLAYERCOMPONENT_H
