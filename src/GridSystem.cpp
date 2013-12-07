#include "GridSystem.h"

#include <iostream>

#include <Fission/Rendering/TransformComponent.h>

#include "PlaceableComponent.h"

GridSystem::GridSystem(EventManager *eventManager, float lockStep) : System(eventManager, lockStep, GridComponent::Type)
{
    //ctor
}

GridSystem::~GridSystem()
{
    //dtor
}

void GridSystem::begin(const float dt)
{
}

void GridSystem::processEntity(Entity *entity, const float dt)
{
    auto trans = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    auto grid = static_cast<GridComponent*>(entity->getComponent(GridComponent::Type));

	// Update all the placeable positions
	for (auto placeableEnt : grid->mPlaceables)
    {
        auto pTrans = reinterpret_cast<TransformComponent*>(placeableEnt->getComponent(TransformComponent::Type));
        auto placeable = reinterpret_cast<PlaceableComponent*>(placeableEnt->getComponent(PlaceableComponent::Type));

        sf::Vector2f pos(placeable->getGridX(), placeable->getGridY());
        pos *= float(TILE_SIZE);
        pos = trans->getTransform().transformPoint(pos);
        pTrans->setPosition(pos);
        pTrans->setRotation(trans->getRotation());
        pTrans->setScale(trans->getScale());
    }

    // ###############
	// Do the grid simulation

	// Iterate through cached interesting tiles that need to be operated on
	for (unsigned int t = 0; t < mTicks.size(); t++)
    {
		if (mTicks[t].mClock.getElapsedTime().asSeconds() < mTicks[t].mDelay)
			continue;

		mTicks[t].mClock.restart();

        if (mTicks[t].mOp.getType() == GridOp::STATIC)
        {
            std::vector<Area> areas;
            for (auto coord : grid->mCTiles[t])
            {
                Area a = grid->getArea(coord.x, coord.y);

                // New tile
                a = mTicks[t].mOp(a);
                if (a.mChanged)
                    areas.push_back(a);
            }

            grid->clearInteresting(t);

            for (auto a : areas)
            {
                grid->setTile(a.mX, a.mY, a.mTiles[1][1], t);
            }
        }
        else
        {
            mTicks[t].mOp(grid, t);
        }
	}
}

void GridSystem::end(const float dt)
{
}
