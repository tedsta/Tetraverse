#ifndef GRIDOPS_H
#define GRIDOPS_H

#include "GridSystem.h"

Area veggyGridOp(Area a);
Area wireGridOp(Area a);
Area fluidGridOp(Area a);
void fluidGridOp2(GridComponent* grid, int tick);

#endif // GRIDOPS_H
