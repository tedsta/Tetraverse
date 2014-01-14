#ifndef BLOCK_H
#define BLOCK_H

#include <SFML/Config.hpp>

#include <Fission/Core/Component.h>

class Block : public fsn::Component
{
    FISSION_COMPONENT

    public:
        Block(sf::Uint16 mat = 0, sf::Uint8 edgeState = 0) : mMat(mat), mEdgeState(edgeState) {}
        ~Block() {}

        sf::Uint16 getMat() const {return mMat;}
        sf::Uint8 getEdgeState() const {return mEdgeState;}

    private:
        sf::Uint16 mMat;
        sf::Uint8 mEdgeState;
};

#endif // BLOCK_H
