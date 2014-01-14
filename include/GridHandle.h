#ifndef GRIDHANDLE_H
#define GRIDHANDLE_H

#include <vector>

#include "GridData.h"

class GridChunk;

class GridHandle
{
    public:
        GridHandle(IGridData* gridData);
        ~GridHandle();

        // Getters
        int getWidth() const {return mGridData->getWidth();}
        int getHeight() const {return mGridData->getHeight();}
        GridChunk* getChunk(int x, int y) const {return mChunks[y*mChunkWidth + x];}
        bool isLarge() const {return mIsLarge;}

    private:
        IGridData* mGridData;

        // Dimensions of grid in chunks
        int mChunkWidth;
        int mChunkHeight;

        // Fake 2D array of chunks
        std::vector<GridChunk*> mChunks;

        // Whether or not this grid is composed of multiple chunks
        bool mIsLarge;
};

#endif // GRIDHANDLE_H
