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

    Entity* oldPrimary = phys->mPrimaryGrid;
    phys->mPrimaryGrid = NULL;
    phys->mGrids.clear(); // Clear all the grids he wants to check collisions with

    int smallestArea = 2000000000; // 2 billion is a pretty big number
    for (auto g : mGridSys->getActiveEntities())
    {
        auto ggrid = reinterpret_cast<GridComponent*>(g->getComponent(GridComponent::Type));

        int area = ggrid->getSizeX()*ggrid->getSizeY();
        if (area < smallestArea && ggrid->contains(trans, sf::Vector2f(phys->mWidth, phys->mHeight)))
        {
            smallestArea = area;
            phys->mPrimaryGrid = g;
            phys->mGrids.push_back(g);
        }
        else if (ggrid->intersects(trans, sf::Vector2f(phys->mWidth*2, phys->mHeight*2)))
            phys->mGrids.push_back(g);
    }

    if (phys->mPrimaryGrid == NULL)
    {
        trans->move(phys->mVelocity*dt);
        //phys->mRelTransform.setPosition(trans->getPosition());
        //phys->mRelTransform.setRotation(trans->getRotation());
        //phys->mRelTransform.setScale(trans->getScale());
        return;
    }

    phys->mDirCollisions[0] = false;
    phys->mDirCollisions[1] = false;
    phys->mDirCollisions[2] = false;
    phys->mDirCollisions[3] = false;

    // Do gravity
    sf::Vector2f gravity;
    gravity.y = 4 * 9.8 * 16;
    phys->mVelocity.y += gravity.y * dt;

    sf::Vector2f velocity = phys->mVelocity;
    if (phys->mPrimaryGrid)
    {
        auto ggrid = reinterpret_cast<GridComponent*>(phys->mPrimaryGrid->getComponent(GridComponent::Type));
        velocity += ggrid->getVelocity();
    }

    trans->move(sf::Vector2f(velocity.x, 0)*dt);
    for (auto curGrid : phys->mGrids)
    {
        auto ggrid = reinterpret_cast<GridComponent*>(curGrid->getComponent(GridComponent::Type));

        int hdir = NO_DIR;
        if (velocity.x-ggrid->getVelocity().x > 0)
            hdir = RIGHT;
        else if (velocity.x-ggrid->getVelocity().x < 0)
            hdir = LEFT;

        // Horizontal
        if (hdir != NO_DIR)
        {
            sf::Vector2f pos = trans->getPosition();
            float fix = 0;
            if (ggrid->checkCollision(trans, sf::Vector2f(phys->getWidth(), phys->getHeight()), hdir, fix))
            {
                if (trans->getPosition().x-pos.x != 0)
                    std::cout << fix << '\t' << trans->getPosition().x-pos.x << std::endl;
                phys->mVelocity.x = 0;
                trans->move(sf::Vector2f(fix, 0));
                phys->mDirCollisions[hdir] = true;
            }
        }
    }

    trans->move(sf::Vector2f(0, velocity.y)*dt);
    for (auto curGrid : phys->mGrids)
    {
        auto ggrid = reinterpret_cast<GridComponent*>(curGrid->getComponent(GridComponent::Type));

        int vdir = NO_DIR;
        if (velocity.y-ggrid->getVelocity().y > 0)
            vdir = DOWN;
        else if (velocity.y-ggrid->getVelocity().y < 0)
            vdir = UP;

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

void PhysicsSystem::calculateLocalTransform(PhysicsComponent* phys)
{
    sf::Vector2f pos = phys->mRelTransform.getPosition();
    float rot = phys->mRelTransform.getRotation();
    sf::Vector2f scale = phys->mRelTransform.getScale();

    Entity* curEnt = phys->mPrimaryGrid;
    while (curEnt)
    {
        auto curPhys = reinterpret_cast<PhysicsComponent*>(curEnt->getComponent(PhysicsComponent::Type));
        pos = curPhys->mRelTransform.getInverseTransform().transformPoint(pos);
        rot -= curPhys->mRelTransform.getRotation();
        scale.x /= curPhys->mRelTransform.getScale().x;
        scale.y /= curPhys->mRelTransform.getScale().y;
        curEnt = curPhys->mPrimaryGrid;
    }

    phys->mRelTransform.setPosition(pos);
    phys->mRelTransform.setRotation(rot);
    phys->mRelTransform.setScale(scale);
}
