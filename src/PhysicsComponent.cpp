#include "PhysicsComponent.h"

#include <cfloat>

#include "phys/Shape.h"

#include "PhysicsSystem.h"
#include "GridComponent.h"
#include "TetraCollision.h"

TypeBits PhysicsComponent::Type;

PhysicsComponent::PhysicsComponent(float width, float height, float density) : primaryGrid(NULL)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->setBox((width/2.f)/PTU, (height/2.f)/PTU);
    body = new phys::RigidBody(shape, density);
    body->setGravity(sf::Vector2f(0, 40.f));
    body->setFixedRotation();
}

PhysicsComponent::PhysicsComponent(sf::Vector2f* verts, int vertCount, float density) : primaryGrid(NULL)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->set(verts, vertCount);
    body = new phys::RigidBody(shape, density);
    body->setGravity(sf::Vector2f(0, 40.f));
    body->setFixedRotation();
}

PhysicsComponent::PhysicsComponent(GridComponent* gridCmp) : primaryGrid(NULL)
{
    GridShape* shape = new GridShape(gridCmp);
    body = new phys::RigidBody(shape, 1.f);

    if (gridCmp->getWrapX())
    {
        body->setStatic();
        body->setGravity(sf::Vector2f(0, 0));
    }
    else
    {
        body->setGravity(sf::Vector2f(0, 40.f));
    }
}

PhysicsComponent::~PhysicsComponent()
{
    delete body;
}

void PhysicsComponent::serialize(sf::Packet &packet)
{
}

void PhysicsComponent::deserialize(sf::Packet &packet)
{
}

void PhysicsComponent::addGrid(Entity* g)
{
    grids.insert(g);
    recalculatePrimaryGrid();
}

void PhysicsComponent::removeGrid(Entity* g)
{
    PhysicsComponent::grids.erase(g);
    if (g == primaryGrid)
        recalculatePrimaryGrid();
}

void PhysicsComponent::recalculatePrimaryGrid()
{
    int smallestArea = FLT_MAX;
    for (auto ent : grids)
    {
        GridComponent* grid = reinterpret_cast<GridComponent*>(ent->getComponent(GridComponent::Type));

        int area = grid->getSizeX()*grid->getSizeY();
        if (area < smallestArea)
        {
            smallestArea = area;
            primaryGrid = ent;
        }
    }

    PhysicsComponent* phys = reinterpret_cast<PhysicsComponent*>(primaryGrid->getComponent(PhysicsComponent::Type));
    body->setParent(phys->getBody());
}
