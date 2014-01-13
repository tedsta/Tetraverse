#include <UnitTest++/UnitTest++.h>

#include "GridChunk.h"

TEST(GridChunk_CreateGridChunk)
{
    GridChunk chunk(10, 10, 50, 60);
    CHECK(chunk.getX() == 10);
    CHECK(chunk.getY() == 10);
    CHECK(chunk.getWidth() == 50);
    CHECK(chunk.getHeight() == 60);
}

TEST(GridChunk_SetBlock)
{
    GridChunk chunk(0, 0, 10, 10);
    BlockData block;
    block.mat = 42;
    chunk.setBlock(7, 3, block);
    CHECK(chunk.getBlock(7, 3).mat == 42);
}
