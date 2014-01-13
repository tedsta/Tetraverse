#include "GridData.h"

#include <sys/stat.h>
#include <fstream>
#include <cassert>

bool fileExists(const std::string& filename);

GridData::GridData(std::string gridFile) : mFileName(gridFile)
{
    assert(fileExists(gridFile)); // Make sure this file exists.
}

GridData::GridData(std::string gridFile, int width, int height) : mFileName(gridFile),
    mWidth(width), mHeight(height)
{
    assert(!fileExists(gridFile)); // Make sure there isn't a grid about to be overwritten.

    std::fstream file(mFileName.c_str(), std::ios::out|std::ios::binary);

    file << "GRID" << width << height;
    for (int x = 0; x < mWidth; x++)
    {
        for (int y = 0; y < mHeight; y++)
        {
            file << 0;
        }
    }

    file.close();
}

GridData::~GridData()
{
    //dtor
}

void GridData::setBlock(int x, int y, const BlockData& block)
{
    mMutex.lock();

    std::ofstream file(mFileName.c_str(), std::ios::app|std::ios::binary);

    // TODO: Do stuff

    file.close();

    mMutex.unlock();
}

BlockData GridData::getBlock(int x, int y)
{
    mMutex.lock();

    BlockData block;

    mMutex.unlock();
    return block;
}

bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}
