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
        GridChunk(int x, int y);
        ~GridChunk();

        void generateEntities(fsn::EntityManager* entityMgr, BlockEntityRegistry* blockReg);

        // Setters
        void setBlock(int x, int y, const BlockData& block);

        // Getters
        const BlockData& getBlock(int x, int y) const {return mBlocks[y*ChunkSize + x];}
        fsn::EntityRef* getEntityAt(int x, int y) const {return mEntities[y*ChunkSize + x];}
        int getChunkX() const {return mChunkX;}
        int getChunkY() const {return mChunkY;}

    private:
        std::vector<BlockData> mBlocks;
        std::vector<fsn::EntityRef*> mEntities;

        // The chunk coordinates
        int mChunkX;
        int mChunkY;
};

#endif // GRIDCHUNK_H
