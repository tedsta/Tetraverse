#include <UnitTest++/UnitTest++.h>

#include "GridData.h"

TEST(GridData_CreateGrid)
{
    GridData grid(10, 10);
    CHECK(grid.getWidth() == 10);
    CHECK(grid.getHeight() == 10);
}
