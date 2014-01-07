#include <UnitTest++/UnitTest++.h>

#include "GridData.h"

TEST(GridData_CreateFrom2DArray)
{
    int width = 10;
    int height = 10;
    BlockData** blocks = new BlockData*[height];
    for (int i = 0; i < height; i++)
        blocks[i] = new BlockData[width];
    GridData* grid = new GridData(blocks, width, height); // Note: grid now owns blocks' memory.
    CHECK(grid->getBlocks() == blocks);
    delete grid;
}
