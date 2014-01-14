#include "GridChunk.h"

#include <Fission/Core/EntityRef.h>

#include "Components/Block.h"

GridChunk::GridChunk(int x, int y) : mBlocks(ChunkSize*ChunkSize), mEntities(ChunkSize*ChunkSize),
    mChunkX(x), mChunkY(y)
{
    //ctor
}

GridChunk::~GridChunk()
{
    //dtor
}

void GridChunk::generateEntities(fsn::EntityManager* entityMgr, BlockEntityRegistry* blockReg)
{
    for (int x = 0; x < ChunkSize; x++)
    {
        for (int y = 0; y < ChunkSize; y++)
        {
            if (getBlock(x, y).mat == 0)
                continue;

            fsn::EntityRef* entity = entityMgr->getEntityRef(entityMgr->createEntity());
            entity->addComponent(new Block(getBlock(x, y).mat));

            mEntities[y*ChunkSize + x] = entity;
        }
    }
}

void GridChunk::setBlock(int x, int y, const BlockData& block)
{
    mBlocks[y*ChunkSize + x] = block;
}
