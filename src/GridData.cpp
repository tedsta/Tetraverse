#include "GridData.h"

#include <sys/stat.h>
#include <fstream>
#include <cassert>

bool fileExists(const std::string& filename);

GridData::GridData(std::string gridFile) : mFileName(gridFile)
{
    assert(fileExists(gridFile)); // Make sure this file exists.

    std::ifstream file(mFileName.c_str(), std::ios::binary);

    // Check the header
    char header[5];
    header[4] = 0;
    file.read(header, 4);
    assert(std::string(header) == std::string("GRID"));

    // Read the grid dimensions
    file.read(reinterpret_cast<char*>(&mWidth), sizeof(mWidth));
    file.read(reinterpret_cast<char*>(&mHeight), sizeof(mHeight));

    // Finally, close the file
    file.close();
}

GridData::GridData(std::string gridFile, int width, int height) : mFileName(gridFile),
    mWidth(width), mHeight(height)
{
    assert(!fileExists(gridFile)); // Make sure there isn't a grid about to be overwritten.

    BlockData emptyBlock{0, 0, 0}; // a little empty block

    std::ofstream file(mFileName.c_str(), std::ios::binary);

    file << "GRID";
    file.write(reinterpret_cast<char*>(&mWidth), sizeof(mWidth));
    file.write(reinterpret_cast<char*>(&mHeight), sizeof(mHeight));
    for (int x = 0; x < mWidth; x++)
    {
        for (int y = 0; y < mHeight; y++)
        {
            file.write(reinterpret_cast<const char*>(&emptyBlock.mat), sizeof(emptyBlock.mat));
            file.write(reinterpret_cast<const char*>(&emptyBlock.edgeState), sizeof(emptyBlock.edgeState));
            file.write(reinterpret_cast<const char*>(&emptyBlock.veggy), sizeof(emptyBlock.veggy));
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

    std::ofstream file(mFileName.c_str(), std::ios::binary|std::ios::out|std::ios::in);

    // Move cursor to block data position
    file.seekp(12 + (y*mWidth + x)*sizeof(BlockData), std::ios::beg); // header+width+height

    file.write(reinterpret_cast<const char*>(&block.mat), sizeof(block.mat));
    file.write(reinterpret_cast<const char*>(&block.edgeState), sizeof(block.edgeState));
    file.write(reinterpret_cast<const char*>(&block.veggy), sizeof(block.veggy));

    file.close();

    mMutex.unlock();
}

BlockData GridData::getBlock(int x, int y)
{
    mMutex.lock();

    std::ifstream file(mFileName.c_str(), std::ios::binary);

    // Move cursor to block data position
    file.seekg(12 + (y*mWidth + x)*sizeof(BlockData)); // header+width+height

    BlockData block;
    file.read(reinterpret_cast<char*>(&block.mat), sizeof(block.mat));
    file.read(reinterpret_cast<char*>(&block.edgeState), sizeof(block.edgeState));
    file.read(reinterpret_cast<char*>(&block.veggy), sizeof(block.veggy));

    file.close();
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
