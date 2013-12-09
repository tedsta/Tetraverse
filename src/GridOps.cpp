#include "GridOps.h"

#include <iostream>

#include "GridComponent.h"

Area veggyGridOp(Area a)
{
    if (a.mTiles[1][1].mMat == 1 && (a.mTiles[0][1].mMat == 3 || a.mTiles[1][0].mMat == 3 || a.mTiles[1][2].mMat == 3 || a.mTiles[2][1].mMat == 3) &&
		(a.mTiles[0][0].mMat == 0 || a.mTiles[0][1].mMat == 0 || a.mTiles[0][2].mMat == 0 ||
        a.mTiles[1][0].mMat == 0 || a.mTiles[1][2].mMat == 0 ||
        a.mTiles[2][0].mMat == 0 || a.mTiles[2][1].mMat == 0 || a.mTiles[2][2].mMat == 0))
    {
        a.mTiles[1][1].mMat = 3;
        a.mChanged = true;
    }

	return a;
}

bool veggyTest(const Tile& oldTile, const Tile& newTile)
{
    if ((newTile.mMat == 1 || newTile.mMat == 3) && oldTile.mMat != newTile.mMat)
        return true;
    return false;
}

// ******************************************************************************************************

bool isFluid(int mat){return mat==0||mat==4;}

const float MaxMass = 1;
const float MaxCompress = 0.05f; //How much excess water a cell can store, compared to the cell above it
const float MinMass = 0.0001;
const float MinFlow = 0.01;
const float MaxSpeed = 1;
const float MinChange = 0.0001;

//Returns the amount of water that should be in the bottom cell.
float fluidStableState(float mass)
{
    if (mass <= MaxMass)
        return MaxMass;
    else if (mass < 2*MaxMass + MaxMass*MaxCompress)
        return (float(MaxMass + mass*MaxCompress)/float(MaxMass + (MaxCompress*MaxMass)))*MaxMass;
    else
        return (mass + MaxMass*MaxCompress)/2.f;
}

float constrain( const float& val, const float& minVal, const float& maxVal )
{
    return std::max( minVal, std::min( val, maxVal ) );
}

void fluidGridOp(GridComponent* grid, int tick)
{
    //return;

    enum {
        UP = 0,
        LEFT = 1,
        CENTER = 2,
        RIGHT = 3,
        DOWN = 4
    };

    struct fluidArea {
        float& operator[](int i){return fluid[i];}
        int x;
        int y;
        float fluid[5];
    };

    std::vector<fluidArea> areas(grid->getInterestingTiles(tick).size()); // up, left, center, right, down

    for (int i = 0; i < grid->getInterestingTiles(tick).size(); i++)
    {
        int x = grid->getInterestingTiles(tick)[i].x;
        int y = grid->getInterestingTiles(tick)[i].y;

        float area[5] = {0.f, 0.f, 0.f, 0.f, 0.f};

        if (y > 0)
            area[UP] = grid->getTile(x, y-1).mFluid;
        area[LEFT] = grid->getTile(grid->wrapX(x-1), y).mFluid;
        area[CENTER] = grid->getTile(x, y).mFluid;
        area[RIGHT] = grid->getTile(grid->wrapX(x+1), y).mFluid;
        if (y < grid->getSizeY()-1)
            area[DOWN] = grid->getTile(x, y+1).mFluid;

        areas[i].x = x;
        areas[i].y = y;
        areas[i][0] = area[0];
        areas[i][1] = area[1];
        areas[i][2] = area[2];
        areas[i][3] = area[3];
        areas[i][4] = area[4];
    }

    for (int i = 0; i < grid->getInterestingTiles(tick).size(); i++)
    {
        int x = grid->getInterestingTiles(tick)[i].x;
        int y = grid->getInterestingTiles(tick)[i].y;

        bool upSolid = true, downSolid = true, leftSolid = true, rightSolid = true, centerSolid = true;
        if (y > 0)
            upSolid = !isFluid(grid->getTile(x, y-1).mMat);
        leftSolid = !isFluid(grid->getTile(grid->wrapX(x-1), y).mMat);
        centerSolid = !isFluid(grid->getTile(x, y).mMat);
        rightSolid = !isFluid(grid->getTile(grid->wrapX(x+1), y).mMat);
        if (y < grid->getSizeY()-1)
            downSolid = !isFluid(grid->getTile(x, y+1).mMat);

        float upFluid, downFluid, leftFluid, rightFluid, centerFluid;
        upFluid = areas[i][UP];
        leftFluid = areas[i][LEFT];
        centerFluid = areas[i][CENTER];
        rightFluid = areas[i][RIGHT];
        downFluid = areas[i][DOWN];

        // Skip inert ground blocks
        if (centerSolid)
            continue;

        // Custom push-only flow
        float Flow = 0;
        float remaining_mass = centerFluid;
        if( remaining_mass <= 0 )
            continue;

        // The block below this one
        if( !downSolid )
        {
            Flow = fluidStableState( remaining_mass + downFluid ) - downFluid;
            if( Flow > MinFlow )
            {
                //leads to smoother flow
                Flow *= 0.5f;
            }
            Flow = constrain( Flow, 0, std::min(MaxSpeed, remaining_mass) );

            grid->addFluid(x, y, -Flow);
            grid->addFluid(x, y+1, Flow);
            remaining_mass -= Flow;
        }

        if ( remaining_mass <= 0 )
            continue;

        // Left
        if ( !leftSolid )
        {
            // Equalize the amount of water in this block and it's neighbour
            Flow = ( centerFluid - leftFluid ) / 4;
            if ( Flow > MinFlow )
            {
                Flow *= 0.5f;
            }
            Flow = constrain(Flow, 0, remaining_mass);
            grid->addFluid(x, y, -Flow);
            grid->addFluid(x-1, y, Flow);
            remaining_mass -= Flow;
        }

        if ( remaining_mass <= 0 )
            continue;

        // Right
        if ( !rightSolid )
        {
            // Equalize the amount of water in this block and it's neighbour
            Flow = ( centerFluid - rightFluid ) / 4;
            if ( Flow > MinFlow )
            {
                Flow *= 0.5f;
            }
            Flow = constrain(Flow, 0, remaining_mass);
            grid->addFluid(x, y, -Flow);
            grid->addFluid(x+1, y, Flow);
            remaining_mass -= Flow;
        }

        if ( remaining_mass <= 0 )
            continue;

        // The block above this one
        if( !upSolid )
        {
            Flow = remaining_mass - fluidStableState( remaining_mass + upFluid );
            if( Flow > MinFlow )
            {
                //leads to smoother flow
                Flow *= 0.5f;
            }
            Flow = constrain( Flow, 0, std::min(MaxSpeed, remaining_mass) );

            grid->addFluid(x, y, -Flow);
            grid->addFluid(x, y-1, Flow);
            remaining_mass -= Flow;
        }
    }

    grid->clearInteresting(tick);
    for (int i = 0; i < areas.size(); i++)
    {
        int x = areas[i].x;
        int y = areas[i].y;
        int left = grid->wrapX(x-1);
        int right = grid->wrapX(x+1);

        if (y > 0 && abs(grid->getTile(x, y-1).mFluid - areas[i][UP]) > MinChange)
            grid->setInteresting(x, y-1, tick);
        if (grid->getTile(left, y).mFluid != areas[i][LEFT])
            grid->setInteresting(left, y, tick);
        if (grid->getTile(x, y).mFluid != areas[i][CENTER])
            grid->setInteresting(x, y, tick);
        if (grid->getTile(right, y).mFluid != areas[i][RIGHT])
            grid->setInteresting(right, y, tick);
        if (y < grid->getSizeY()-1 && grid->getTile(x, y+1).mFluid != areas[i][DOWN])
            grid->setInteresting(x, y+1, tick);
    }
}

bool fluidTest(const Tile& oldTile, const Tile& newTile)
{
    if (oldTile.mFluid != newTile.mFluid)
        return true;
    return false;
}

// ************************************************************************************************

Area lightGridOp(Area a)
{
    int light = std::max(a.mTiles[0][1].mLight, std::max(a.mTiles[1][0].mLight, std::max(a.mTiles[1][2].mLight, a.mTiles[2][1].mLight)));
    if (light-1 < a.mTiles[1][1].mLight)
        return a;


    a.mTiles[1][1].mLight = light-1;
    if (a.mTiles[1][1].mMat != 0 && a.mTiles[1][1].mLight > 0)
        a.mTiles[1][1].mLight--;

    a.mChanged = true;

	return a;
}

bool lightTest(const Tile& oldTile, const Tile& newTile)
{
    if (oldTile.mLight != newTile.mLight)
        return true;
    return false;
}
