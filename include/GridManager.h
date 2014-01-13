#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include <vector>

class GridData;

class GridManager
{
    public:
        GridManager();
        ~GridManager();

        GridData* createGrid(int width, int height);

    private:
        std::vector<GridData*> mGrids;

        int mNextID;
};

#endif // GRIDMANAGER_H
