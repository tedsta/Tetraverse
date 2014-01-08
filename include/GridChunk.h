#ifndef GRIDCHUNK_H
#define GRIDCHUNK_H

#include <vector>

#include "GridData.h"

class GridChunk
{
    public:
        GridChunk();
        ~GridChunk();

    private:
        std::vector<BlockData> mBlocks;

        // The coordinates of the top left corner of this chunk in its grid.
        int mX;
        int mY;

        // The block dimensions of this chunk.
        int mWidth;
        int mHeight;
};

#endif // GRIDCHUNK_H
