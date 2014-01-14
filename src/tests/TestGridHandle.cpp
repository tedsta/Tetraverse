#include <UnitTest++/UnitTest++.h>

#include "GridData.h"
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
    GridHandleFixture() : grid(new MockGridData(10, 10))
    {
    }

    GridHandle grid;
};

TEST_FIXTURE(GridHandleFixture, GridHandle_Create)
{
    CHECK(grid.getWidth() == 10);
    CHECK(grid.getHeight() == 10);
}
