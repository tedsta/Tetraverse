#ifndef BLOCK_H
#define BLOCK_H

#include <SFML/Config.hpp>

#include <Fission/Core/Component.h>

class Block : public fsn::Component
{
    FISSION_COMPONENT

    public:
        Block(sf::Uint16 mat = 0) : mMat(mat) {}
        ~Block() {}

        sf::Uint16 getMat() const {return mMat;}

    private:
        sf::Uint16 mMat;
};

#endif // BLOCK_H
