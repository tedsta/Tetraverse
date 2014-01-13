#ifndef GRIDCHUNK_H
#define GRIDCHUNK_H

#include <vector>

#include <Fission/Core/EntityManager.h>

#include "GridData.h"

const int ChunkSize = 150; // 150x150 chunks

class BlockEntityRegistry;

class GridChunk
{
    public:
        GridChunk(int x, int y, int width, int height);
        ~GridChunk();

        void generateEntities(fsn::EntityManager* entityMgr, BlockEntityRegistry* blockReg);

        // Setters
        void setBlock(int x, int y, const BlockData& block);

        // Getters
        const BlockData& getBlock(int x, int y) const {return mBlocks[y*mWidth + x];}
        int getX() const {return mX;}
        int getY() const {return mY;}
        int getWidth() const {return mWidth;}
        int getHeight() const {return mHeight;}

    private:
        std::vector<BlockData> mBlocks;

        // The coordinates of the top left corner of this chunk in its grid.
        int mX;
        int mY;

        // The block dimensions of this chunk.
        int mWidth;
        int mHeight;
};

#endif // GRIDCHUNK_H
