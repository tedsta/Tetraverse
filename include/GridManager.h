#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include <vector>

class IGridData;

class GridManager
{
    public:
        GridManager();
        ~GridManager();

        IGridData* createGrid(int width, int height);

    private:
        std::vector<IGridData*> mGrids;

        int mNextID;
};

#endif // GRIDMANAGER_H
