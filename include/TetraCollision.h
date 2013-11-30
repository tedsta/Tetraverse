#ifndef TETRA_COLLISION_H
#define TETRA_COLLISION_H

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

sf::Vector2f calcSupportPoint(sf::Vector2f* vertices, int vertexCount, sf::Vector2f dir);
void gridToPolygon(phys::Manifold *m, phys::RigidBody *a, phys::RigidBody *b);
void polygonToGrid(phys::Manifold *m, phys::RigidBody *a, phys::RigidBody *b);

class GridShape : public phys::Shape
{
    public:
        GridShape(GridComponent* _grid) : grid(_grid) {}

        void computeMass(phys::RigidBody* body, float density)
        {
        }

        GridComponent* getGrid(){return grid;}
        int getType() const {return ::Shape::Grid;}

    private:
        GridComponent* grid;
};

#endif // COLLISION_H
