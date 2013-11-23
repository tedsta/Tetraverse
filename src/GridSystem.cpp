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

	// ###############
	// Do the grid simulation

	if (grid->mID == -1)
    {
		grid->mID = mNextGridID;
		mNextGridID++;
	}

	// Update all the placeable positions
	for (auto placeableEnt : grid->mPlaceables)
    {
        auto trans = reinterpret_cast<TransformComponent*>(placeableEnt->getComponent(TransformComponent::Type));
        auto placeable = reinterpret_cast<PlaceableComponent*>(placeableEnt->getComponent(PlaceableComponent::Type));

        sf::Vector2f pos(placeable->getGridX(), placeable->getGridY());
        pos *= float(TILE_SIZE);
        pos = grid->mTransform->getTransform().transformPoint(pos);
        trans->setPosition(pos);
        trans->setRotation(grid->mTransform->getRotation());
        trans->setScale(grid->mTransform->getScale());
    }

	// Iterate through cached interesting tiles that need to be operated on
	for (unsigned int t = 0; t < mTicks.size(); t++)
    {
		if (mTicks[t].mClock.getElapsedTime().asSeconds() < mTicks[t].mDelay)
			continue;

		mTicks[t].mClock.restart();

        //if (grid->mCTiles[t].size())
        //    std::cout << "Tick!\n";

        //std::cout << grid->mCTiles[t].size() << std::endl;
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

            //if (areas.size() > 0)
            //    std::cout << "Areas: " << areas.size() << std::endl;
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
