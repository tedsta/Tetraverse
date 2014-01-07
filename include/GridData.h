#ifndef GRIDDATA_H
#define GRIDDATA_H

#include <vector>

#include <SFML/Config.hpp>

struct BlockData
{
    sf::Uint16 mat;
    sf::Uint8 edgeState;
    sf::Uint8 veggy;
};

class GridData
{
    public:
        GridData(BlockData** blocks, int width, int height);
        ~GridData();

        BlockData** getBlocks(){return mBlocks;}

    private:
        BlockData** mBlocks;
        int mWidth;
        int mHeight;
};

#endif // GRIDDATA_H
