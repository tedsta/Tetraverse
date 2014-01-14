#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include <vector>

class GridHandle;

class GridManager
{
    public:
        GridManager();
        ~GridManager();

        GridHandle* createGrid(int width, int height);

    private:
        std::vector<GridHandle*> mGrids;

        int mNextID;
};

#endif // GRIDMANAGER_H
