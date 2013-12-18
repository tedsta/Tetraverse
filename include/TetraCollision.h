#ifndef TETRA_COLLISION_H
#define TETRA_COLLISION_H

#include <cfloat>
#include <SFML/Network/Packet.hpp>

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

void gridToCircle(phys::Collision* c, phys::RigidBody *a, phys::RigidBody *b);
void circleToGrid(phys::Collision* c, phys::RigidBody *a, phys::RigidBody *b);
void gridToPolygon(phys::Collision* c, phys::RigidBody *a, phys::RigidBody *b);
void polygonToGrid(phys::Collision* c, phys::RigidBody *a, phys::RigidBody *b);
void gridToGrid(phys::Collision* c, phys::RigidBody *a, phys::RigidBody *b);

class GridShape : public phys::Shape
{
    public:
        GridShape(GridComponent* _grid) : grid(_grid)
        {
            if (grid)
            {
                boundingBox.left = 0;
                boundingBox.top = 0;
                boundingBox.width = grid->getSizeX();
                boundingBox.height = grid->getSizeY();
            }
        }

        void serialize(sf::Packet &packet)
        {
            packet << grid->getID();
        }

        void deserialize(sf::Packet &packet)
        {
            int gridID;
            packet >> gridID;
            grid = reinterpret_cast<GridComponent*>(Component::get(gridID));

            if (grid)
            {
                boundingBox.left = 0;
                boundingBox.top = 0;
                boundingBox.width = grid->getSizeX();
                boundingBox.height = grid->getSizeY();
            }
        }

        void computeMass(phys::RigidBody* body, float density)
        {
            body->setMass(1000.f);
            body->setInverseMass(0.001f);
            body->setInertia(0.f);
            body->setInverseInertia(0.f);

            /*body->setMass(density * area);
            body->setInverseMass((body->getMass()) ? 1.0f / body->getMass() : 0.0f);
            body->setInertia(I * density);
            body->setInverseInertia(body->getInertia() ? 1.0f / body->getInertia() : 0.0f);*/
        }

        void setRotation(float rot)
        {
            u.set(rot);

            boundingBox.left = 0;
            boundingBox.top = 0;
            boundingBox.width = grid->getSizeX();
            boundingBox.height = grid->getSizeY();

            if (grid->getWrapX())
            {
                boundingBox.left = -FLT_MAX/2;
                boundingBox.width = FLT_MAX;
            }
        }

        GridComponent* getGrid(){return grid;}
        int getType() const {return ::Shape::Grid;}

    private:
        GridComponent* grid;
};

#endif // COLLISION_H
