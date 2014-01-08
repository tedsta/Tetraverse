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
        GridData(int width, int height);
        ~GridData();

        int getWidth() const {return mWidth;}
        int getHeight() const {return mHeight;}

    private:
        int mWidth;
        int mHeight;
};

#endif // GRIDDATA_H
