#include <UnitTest++/UnitTest++.h>

#include "GridData.h"
#include "GridChunk.h"
#include "GridHandle.h"

/// \brief Mock the GridData class, which facilitates access to grid data files.
class MockGridData : public IGridData
{
    public:
        MockGridData(int width, int height) : mBlocks(width*height), mWidth(width), mHeight(height)
        {
        }

        // Setters
        void setBlock(int x, int y, const BlockData& block)
        {
            mBlocks[y*mWidth + x] = block;
        }

        // Getters
        BlockData getBlock(int x, int y)
        {
            return mBlocks[y*mWidth + x];
        }

        int getWidth() const
        {
            return mWidth;
        }

        int getHeight() const
        {
            return mHeight;
        }

    private:
        std::vector<BlockData> mBlocks;
        int mWidth;
        int mHeight;
};

struct GridHandleFixture
{
    GridHandleFixture() : grid(new MockGridData(ChunkSize*2, ChunkSize*2))
    {
    }

    GridHandle grid;
};

TEST_FIXTURE(GridHandleFixture, GridHandle_Create)
{
    CHECK(grid.getWidth() == ChunkSize*2);
    CHECK(grid.getHeight() == ChunkSize*2);
}

TEST_FIXTURE(GridHandleFixture, GridHandle_IsLarge)
{
    CHECK(grid.isLarge()); // Large one

    // Small one
    GridHandle smallGrid(new MockGridData(ChunkSize, ChunkSize-1));
    CHECK(!smallGrid.isLarge());
}

TEST_FIXTURE(GridHandleFixture, GridHandle_ChunksAreNull)
{
    CHECK(grid.getChunk(0, 0) == nullptr);
    CHECK(grid.getChunk(0, 1) == nullptr);
    CHECK(grid.getChunk(1, 0) == nullptr);
    CHECK(grid.getChunk(1, 1) == nullptr);
}
