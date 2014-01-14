#include <UnitTest++/UnitTest++.h>

#include "GridData.h"
#include "GridHandle.h"

class MockGridData;

struct GridHandleFixture
{
    GridHandleFixture()
    {
    }

    MockGridData* data;
    GridHandle handle;
};

class MockGridData : public IGridData
{
    public:
        MockGridData(int width, int height)
        {
        }

    private:
};

TEST_FIXTURE(GridHandleFixture, GridHandle_Create)
{
    CHECK(false);
}
