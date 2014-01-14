#include "GridChunk.h"

#include <Fission/Core/EntityRef.h>

#include "Components/Block.h"

GridChunk::GridChunk(int x, int y, int width, int height) : mBlocks(width*height), mEntities(width*height),
    mChunkX(x), mChunkY(y), mWidth(width), mHeight(height)
{
    //ctor
}

GridChunk::~GridChunk()
{
    //dtor
}

void GridChunk::generateEntities(fsn::EntityManager* entityMgr)
{
    for (int x = 0; x < ChunkSize; x++)
    {
        for (int y = 0; y < ChunkSize; y++)
        {
            if (getBlock(x, y).mat == 0)
                continue;

            fsn::EntityRef* entity = entityMgr->getEntityRef(entityMgr->createEntity());
            entity->addComponent(new Block(getBlock(x, y).mat, getBlock(x, y).edgeState));

            mEntities[y*ChunkSize + x] = entity;
        }
    }
}

void GridChunk::destroyEntities()
{
    for (int x = 0; x < ChunkSize; x++)
    {
        for (int y = 0; y < ChunkSize; y++)
        {
            mEntities[y*ChunkSize + x]->destroy();
        }
    }
}

void GridChunk::setBlock(int x, int y, const BlockData& block)
{
    mBlocks[y*mWidth + x] = block;
}
