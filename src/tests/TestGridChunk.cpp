#include <UnitTest++/UnitTest++.h>

#include "GridChunk.h"

struct GridChunkFixture
{
    GridChunkFixture() : chunk(5, 5, 15, 10)
    {
    }

    ~GridChunkFixture()
    {
    }

    GridChunk chunk;
};

TEST_FIXTURE(GridChunkFixture, GridChunkCreation)
{
    CHECK(chunk.getX() == 5);
    CHECK(chunk.getY() == 5);
    CHECK(chunk.getWidth() == 15);
    CHECK(chunk.getHeight() == 10);
}

TEST_FIXTURE(GridChunkFixture, SetBlock)
{
    BlockData block;
    block.mat = 42;
    chunk.setBlock(7, 3, block);
    CHECK(chunk.getBlock(7, 3).mat == 42);
}
