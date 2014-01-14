#include "GridHandle.h"

#include <algorithm> // for std::max

#include "GridChunk.h"

GridHandle::GridHandle(IGridData* gridData) : mGridData(gridData),
    mChunkWidth(std::max(mGridData->getWidth()/ChunkSize, 1)), mChunkHeight(std::max(mGridData->getHeight()/ChunkSize, 1)),
    mChunks(mChunkWidth*mChunkHeight, nullptr), mIsLarge(mChunkWidth > 1 || mChunkHeight > 1)
{
    //ctor
}

GridHandle::~GridHandle()
{
    delete mGridData;
}
