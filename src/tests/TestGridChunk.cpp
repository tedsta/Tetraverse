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
