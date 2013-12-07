#ifndef GRIDOPS_H
#define GRIDOPS_H

#include "GridSystem.h"

Area veggyGridOp(Area a);
bool veggyTest(const Tile& oldTile, const Tile& newTile);

void fluidGridOp(GridComponent* grid, int tick);
bool fluidTest(const Tile& oldTile, const Tile& newTile);

#endif // GRIDOPS_H
