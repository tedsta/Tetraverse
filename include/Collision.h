#ifndef COLLISION_H
#define COLLISION_H

#include "phys/Manifold.h"
#include "phys/RigidBody.h"
#include "phys/Shape.h"

#include "GridComponent.h"

// Pixels to units ratio
const float PTU = 16.f;

namespace Shape
{
    enum
    {
        Grid = phys::Shape::Custom
    };
}

void gridToPolygon(phys::Manifold *m, phys::RigidBody *a, phys::RigidBody *b);

class GridShape : public phys::Shape
{
    public:
        GridShape(GridComponent* _grid) : grid(_grid) {}

        GridComponent* getGrid(){return grid;}

    private:
        GridComponent* grid;
};

#endif // COLLISION_H
