#include "GridChunk.h"

GridChunk::GridChunk(int x, int y, int width, int height) : mBlocks(width*height), mX(x), mY(y),
    mWidth(width), mHeight(height)
{
    //ctor
}

GridChunk::~GridChunk()
{
    //dtor
}

void GridChunk::setBlock(int x, int y, const BlockData& block)
{
    mBlocks[y*mWidth + x] = block;
}
