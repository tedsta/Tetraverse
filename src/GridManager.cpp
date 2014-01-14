#include "GridManager.h"

#include <sstream>

#include "GridHandle.h"

GridManager::GridManager() : mNextID(0)
{
    //ctor
}

GridManager::~GridManager()
{
    //dtor
}

GridHandle* GridManager::createGrid(int width, int height)
{
    std::string strID = static_cast<std::ostringstream*>( &(std::ostringstream() << mNextID) )->str(); // Convert mNextID to string
    GridHandle* grid = new GridHandle(new GridData(std::string("grid")+strID+".tvg", width, height));
    mGrids.push_back(grid);
    mNextID++;

    return grid;
}
