#ifndef GRIDHANDLE_H
#define GRIDHANDLE_H

#include "GridData.h"

class GridHandle
{
    public:
        GridHandle(IGridData* gridData);
        ~GridHandle();

        // Getters
        int getWidth() const {return mGridData->getWidth();}
        int getHeight() const {return mGridData->getHeight();}

    private:
        IGridData* mGridData;
};

#endif // GRIDHANDLE_H
