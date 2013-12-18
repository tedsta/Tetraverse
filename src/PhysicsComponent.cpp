#include "PhysicsComponent.h"

#include <cfloat>

#include "phys/Shape.h"

#include "PhysicsSystem.h"
#include "GridComponent.h"
#include "TetraCollision.h"

TypeBits PhysicsComponent::Type;

PhysicsComponent::PhysicsComponent(int width, int height, float _density) : primaryGrid(NULL), density(_density)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->setBox((float(width)/2.f)/PTU, (float(height)/2.f)/PTU);
    body = new phys::RigidBody(shape, 2, density);
    body->setGravity(sf::Vector2f(0, 40.f));
    body->setFixedRotation();
}

PhysicsComponent::PhysicsComponent(float radius, float _density) : primaryGrid(NULL), density(_density)
{
    phys::CircleShape* shape = new phys::CircleShape(radius);
    body = new phys::RigidBody(shape, 2, density);
    body->setGravity(sf::Vector2f(0, 40.f));
    body->setFixedRotation();
    body->setRestitution(0.f);
    body->setDynamicFriction(0.f);
    body->setStaticFriction(0.f);
}

PhysicsComponent::PhysicsComponent(sf::Vector2f* verts, int vertCount, float _density) : primaryGrid(NULL), density(_density)
{
    phys::PolygonShape* shape = new phys::PolygonShape();
    shape->set(verts, vertCount);
    body = new phys::RigidBody(shape, 2, density);
    body->setGravity(sf::Vector2f(0, 40.f));
    body->setFixedRotation();
}

PhysicsComponent::PhysicsComponent(GridComponent* gridCmp) : primaryGrid(NULL)
{
    GridShape* shape = new GridShape(gridCmp);
    body = new phys::RigidBody(shape, (gridCmp->getWrapX() ? 0 : 1), 1.f);

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
    packet << body->getShape()->getType();
    body->getShape()->serialize(packet);

    packet << body->getType();
    packet << density;
    packet << body->getGravity().x << body->getGravity().y;
}

void PhysicsComponent::deserialize(sf::Packet &packet)
{
    phys::Shape* shape;

    int shapeType;
    packet >> shapeType;
    switch (shapeType)
    {
    case phys::Shape::Circle:
        {
            shape = new phys::CircleShape(0);
            break;
        }
    case phys::Shape::Polygon:
        {
            shape = new phys::PolygonShape;
            break;
        }
    case Shape::Grid:
        {
            shape = new GridShape(NULL);
            break;
        }
    default:
        {
            return;
        }
    }

    shape->deserialize(packet);

    int type;
    sf::Vector2f gravity;
    packet >> type;
    packet >> density;
    packet >> gravity.x >> gravity.y;

    body = new phys::RigidBody(shape, type, density);
    body->setGravity(gravity);

    switch (shapeType)
    {
    case phys::Shape::Circle:
    case phys::Shape::Polygon:
        {
            body->setFixedRotation();
            break;
        }
    case Shape::Grid:
        {
            if (reinterpret_cast<GridShape*>(body->getShape())->getGrid()->getWrapX())
            {
                body->setStatic();
            }
            break;
        }
    }
}

void PhysicsComponent::postDeserialize()
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
    primaryGrid = NULL;
    body->setParent(NULL);

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

    if (primaryGrid)
    {
        PhysicsComponent* phys = reinterpret_cast<PhysicsComponent*>(primaryGrid->getComponent(PhysicsComponent::Type));
        body->setParent(phys->getBody());
    }
}
