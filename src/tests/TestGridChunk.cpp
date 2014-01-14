#include <UnitTest++/UnitTest++.h>

#include "GridChunk.h"

struct GridChunkFixture
{
    GridChunkFixture() : chunk(1, 1, 15, 10)
    {
    }

    ~GridChunkFixture()
    {
    }

    GridChunk chunk;
};

TEST_FIXTURE(GridChunkFixture, GridChunkCreation)
{
    CHECK(chunk.getChunkX() == 1);
    CHECK(chunk.getChunkY() == 1);
}

TEST_FIXTURE(GridChunkFixture, SetBlock)
{
    BlockData block;
    block.mat = 42;
    chunk.setBlock(7, 3, block);
    CHECK(chunk.getBlock(7, 3).mat == 42);
}
