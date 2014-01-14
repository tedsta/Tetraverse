#include "GridHandle.h"

GridHandle::GridHandle(IGridData* gridData) : mGridData(gridData)
{
    //ctor
}

GridHandle::~GridHandle()
{
    delete mGridData;
}
