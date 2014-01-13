#ifndef GRIDDATA_H
#define GRIDDATA_H

#include <vector>

#include <SFML/Config.hpp>
#include <SFML/System/Mutex.hpp>

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

        // Getters
        BlockData getBlock(int x, int y) const;
        int getWidth() const {return mWidth;}
        int getHeight() const {return mHeight;}

    private:
        int mWidth;
        int mHeight;

        sf::Mutex mMutex; // For threadsafe grid access
};

#endif // GRIDDATA_H