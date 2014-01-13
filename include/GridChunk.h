#ifndef GRIDCHUNK_H
#define GRIDCHUNK_H

#include <vector>

#include "GridData.h"

class GridChunk
{
    public:
        GridChunk(int x, int y, int width, int height);
        ~GridChunk();

        // Getters
        int getX() const {return mX;}
        int getY() const {return mY;}
        int getWidth() const {return mWidth;}
        int getHeight() const {return mHeight;}

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
