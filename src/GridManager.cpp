#include "GridManager.h"

#include <sstream>

#include "GridData.h"

GridManager::GridManager() : mNextID(0)
{
    //ctor
}

GridManager::~GridManager()
{
    //dtor
}

GridData* GridManager::createGrid(int width, int height)
{
    std::string strID = static_cast<std::ostringstream*>( &(std::ostringstream() << mNextID) )->str(); // Convert mNextID to string
    GridData* grid = new GridData(std::string("grid")+strID+".tvg", width, height);
    mGrids.push_back(grid);
    mNextID++;

    return grid;
}
