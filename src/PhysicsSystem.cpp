#include "PhysicsSystem.h"

#include <Fission/Core/Math.h>
#include <Fission/Rendering/TransformComponent.h>

#include "GridSystem.h"
#include "GridComponent.h"
#include "PhysicsComponent.h"

PhysicsSystem::PhysicsSystem(EventManager *eventManager, GridSystem* gridSys) :
    System(eventManager, TransformComponent::Type|PhysicsComponent::Type, 0), mGridSys(gridSys)
{
    //ctor
}

PhysicsSystem::~PhysicsSystem()
{
    //dtor
}

void PhysicsSystem::begin(const float dt)
{
}

void PhysicsSystem::processEntity(Entity *entity, const float dt)
{
    auto trans = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    auto phys = reinterpret_cast<PhysicsComponent*>(entity->getComponent(PhysicsComponent::Type));
    auto grid = reinterpret_cast<GridComponent*>(entity->getComponent(GridComponent::Type));

    phys->mPrimaryGrid = NULL;
    phys->mGrids.clear(); // Clear all the grids he wants to check collisions with

    int smallestArea = 2000000000; // 2 billion is a pretty big number
    for (auto g : mGridSys->getActiveEntities())
    {
        if (g == entity)
            continue;

        auto gt = reinterpret_cast<TransformComponent*>(g->getComponent(TransformComponent::Type));
        auto gphys = reinterpret_cast<PhysicsComponent*>(g->getComponent(PhysicsComponent::Type));
        auto ggrid = reinterpret_cast<GridComponent*>(g->getComponent(GridComponent::Type));

        if (grid && grid->contains(gt, sf::Vector2f(gphys->mWidth, gphys->mHeight)))
            continue;

        auto dist = gt->getPosition() - gt->getOrigin() - trans->getPosition();
        if (length(dist) < 20000*16)
        {
            int area = ggrid->getSizeX()*ggrid->getSizeY();
            if (area < smallestArea && ggrid->contains(trans, sf::Vector2f(phys->mWidth, phys->mHeight)))
            {
                smallestArea = area;
                phys->mPrimaryGrid = g;
            }
            phys->mGrids.push_back(g);
        }
    }

    if (phys->mPrimaryGrid == NULL)
    {
        trans->move(phys->mVelocity*dt);
        return;
    }

    phys->mDirCollisions[0] = false;
    phys->mDirCollisions[1] = false;
    phys->mDirCollisions[2] = false;
    phys->mDirCollisions[3] = false;

    auto gt = reinterpret_cast<TransformComponent*>(phys->mPrimaryGrid->getComponent(TransformComponent::Type));
    auto ggrid = reinterpret_cast<GridComponent*>(phys->mPrimaryGrid->getComponent(GridComponent::Type));
    auto gphys = reinterpret_cast<PhysicsComponent*>(phys->mPrimaryGrid->getComponent(PhysicsComponent::Type));

    // Handle collisions
    int hdir = NO_DIR;
    int vdir = NO_DIR;
    if (gphys->mVelocity.x > 0)
        hdir = RIGHT;
    else if (gphys->mVelocity.x < 0)
        hdir = LEFT;

    if (gphys->mVelocity.y > 0)
        vdir = DOWN;
    else if (gphys->mVelocity.y < 0)
        vdir = UP;

    // Horizontal
    trans->move(gphys->mVelocity*dt);
    /*if (hdir != NO_DIR)
    {
        float fix = 0;
        if (ggrid->checkCollision(trans, sf::Vector2f(phys->getWidth(), phys->getHeight()), hdir, fix))
        {
            phys->mVelocity.x = 0;
            trans->move(sf::Vector2f(fix, 0));
        }
    }

    // Vertical
    trans->move(sf::Vector2f(0, gphys->mVelocity.y)*dt);
    if (vdir != NO_DIR)
    {
        float fix = 0;
        if (ggrid->checkCollision(trans, sf::Vector2f(phys->getWidth(), phys->getHeight()), vdir, fix))
        {
            phys->mVelocity.y = 0;
            trans->move(sf::Vector2f(0, fix));
        }
    }*/

    // Do gravity
    sf::Vector2f gravity;
    gravity.y = 4 * 9.8 * 16;
    phys->mVelocity.y += gravity.y * dt;

    hdir = NO_DIR;
    vdir = NO_DIR;
    if (phys->mVelocity.x > 0)
        hdir = RIGHT;
    else if (phys->mVelocity.x < 0)
        hdir = LEFT;

    if (phys->mVelocity.y > 0)
        vdir = DOWN;
    else if (phys->mVelocity.y < 0)
        vdir = UP;

    trans->move(sf::Vector2f(phys->mVelocity.x, 0)*dt);
    for (auto curGrid : phys->mGrids)
    {
        gt = reinterpret_cast<TransformComponent*>(curGrid->getComponent(TransformComponent::Type));
        ggrid = reinterpret_cast<GridComponent*>(curGrid->getComponent(GridComponent::Type));
        gphys = reinterpret_cast<PhysicsComponent*>(curGrid->getComponent(PhysicsComponent::Type));

        // Horizontal
        if (hdir != NO_DIR)
        {
            float fix = 0;
            if (ggrid->checkCollision(trans, sf::Vector2f(phys->getWidth(), phys->getHeight()), hdir, fix))
            {
                phys->mVelocity.x = 0;
                trans->move(sf::Vector2f(fix, 0));
                phys->mDirCollisions[hdir] = true;
            }
        }
    }

    trans->move(sf::Vector2f(0, phys->mVelocity.y)*dt);
    for (auto curGrid : phys->mGrids)
    {
        gt = reinterpret_cast<TransformComponent*>(curGrid->getComponent(TransformComponent::Type));
        ggrid = reinterpret_cast<GridComponent*>(curGrid->getComponent(GridComponent::Type));
        gphys = reinterpret_cast<PhysicsComponent*>(curGrid->getComponent(PhysicsComponent::Type));

        // Vertical
        if (vdir != NO_DIR)
        {
            float fix = 0;
            if (ggrid->checkCollision(trans, sf::Vector2f(phys->getWidth(), phys->getHeight()), vdir, fix))
            {
                phys->mVelocity.y = 0;
                trans->move(sf::Vector2f(0, fix));
                phys->mDirCollisions[vdir] = true;
            }
        }
    }
}

void PhysicsSystem::end(const float dt)
{
}
