#ifndef PLAYERCOMPONENT_H
#define PLAYERCOMPONENT_H

#include <queue>
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
        const sf::Vector2f& frontLeftCoord(){return mLeftCoords.front();}
        void popLeftCoord(){mLeftCoords.pop();}
        void clearLeftCoords(){while (!mLeftCoords.empty()) mLeftCoords.pop();}

        int getRightCoordsCount(){return mRightCoords.size();}
        void pushRightCoord(sf::Vector2f coord){mRightCoords.push(coord);}
        const sf::Vector2f& frontRightCoord(){return mRightCoords.front();}
        void popRightCoord(){mRightCoords.pop();}
        void clearRightCoords(){while (!mRightCoords.empty()) mRightCoords.pop();}

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
        int mLeftHandState;
        int mRightHandState;
        std::queue<sf::Vector2f> mLeftCoords;
        std::queue<sf::Vector2f> mRightCoords;
};

#endif // PLAYERCOMPONENT_H
