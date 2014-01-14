#ifndef GRIDDATA_H
#define GRIDDATA_H

#include <string>
#include <vector>

#include <SFML/Config.hpp>
#include <SFML/System/Mutex.hpp>

struct BlockData
{
    sf::Uint16 mat;
    sf::Uint8 edgeState;
    sf::Uint8 veggy;
};

class IGridData
{
    public:
        // Setters
        virtual void setBlock(int x, int y, const BlockData& block) = 0;

        // Getters
        virtual BlockData getBlock(int x, int y) = 0;
        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;
};

class GridData : public IGridData
{
    public:
        GridData(std::string gridFile);
        GridData(std::string gridFile, int width, int height);
        ~GridData();

        // Setters
        void setBlock(int x, int y, const BlockData& block);

        // Getters
        BlockData getBlock(int x, int y);
        int getWidth() const {return mWidth;}
        int getHeight() const {return mHeight;}

    private:
        std::string mFileName; // File path to store grid data

        int mWidth;
        int mHeight;

        sf::Mutex mMutex; // For threadsafe grid access
};

#endif // GRIDDATA_H
