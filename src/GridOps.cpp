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

Area wireGridOp(Area a)
{
    int wire = a.mTiles[1][1].mWire;
    int signal = a.mTiles[1][1].mSignal;
    if (wire >= 1)
    {
        if (wire  < a.mTiles[0][1].mWire-1){
            a.mTiles[1][1].mWire = a.mTiles[0][1].mWire;
            a.mTiles[1][1].mSignal = a.mTiles[0][1].mSignal;
            a.mChanged = true;
        }

        if (wire  < a.mTiles[1][0].mWire-1 && a.mTiles[1][1].mWire  < a.mTiles[1][0].mWire) {
            a.mTiles[1][1].mWire = a.mTiles[1][0].mWire;
            a.mTiles[1][1].mSignal = a.mTiles[1][0].mSignal;
            a.mChanged = true;
        }
        if (wire  < a.mTiles[2][1].mWire-1 && a.mTiles[1][1].mWire  < a.mTiles[2][1].mWire){
            a.mTiles[1][1].mWire = a.mTiles[2][1].mWire;
            a.mTiles[1][1].mSignal = a.mTiles[2][1].mSignal;
            a.mChanged = true;
        }
        if (wire  < a.mTiles[1][2].mWire-1 && a.mTiles[1][1].mWire  < a.mTiles[1][2].mWire){
            a.mTiles[1][1].mWire = a.mTiles[1][2].mWire;
            a.mTiles[1][1].mSignal = a.mTiles[1][2].mSignal;
            a.mChanged = true;
        }
    }
    if(!a.mChanged && a.mTiles[1][1].mWire > 1 ){
            a.mTiles[1][1].mWire -= 1;
            a.mChanged = true;
        }
    if(a.mTiles[1][1].mWire <= 1){
        a.mTiles[1][1].mSignal = 0;
    }

	return a;
}

// ******************************************************************************************************

bool isFluid(int mat){return mat==0||mat==4;}

const float MaxMass = 1;
const float MaxCompress = 0.05f; //How much excess water a cell can store, compared to the cell above it
const float MinMass = 0.0001;
const float MinFlow = 0.01;
const float MaxSpeed = 1;

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

void fluidGridOp2(GridComponent* grid, int tick)
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

    for (int i = 0; i < grid->getInterestingTiles(tick).size(); i++)
    {
    }
}

// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************

#define MAX_FLUID 127
#define compress 3

int vert(int top, int bot, int offset)
{
    int total = top+bot;
    int flow = 0;

    int topMaxFill = std::min(MAX_FLUID+offset*compress, 255);
    int botMaxFill = std::min(MAX_FLUID+(offset+1)*compress, 255);
    if (top > 0 && top <= topMaxFill && bot < botMaxFill)
        flow = std::min<int>(top, botMaxFill-bot);
    else if (bot > botMaxFill && top < 255)
        flow = -std::min(bot-botMaxFill, 255-top);

    return flow;
}

int horiz(int p, int s, int po, int so)
{
    int pMaxFill = std::min(MAX_FLUID+po*compress, 255);
    int sMaxFill = std::min(MAX_FLUID+so*compress, 255);
    int flow = 0;

    if (p > s && po > so)
    {
        flow = -compress;
    }
    else if (s > p && so > po)
        flow = compress;

    /*if (top > 0 && top <= topMaxFill && bot < botMaxFill)
        flow = std::min<int>(top, botMaxFill-bot);

    else if (bot > botMaxFill && top < 255)
    {
        //std::cout << "Top: " << top << std::endl << "Bot: " << bot << std::endl << "Max: " << maxFill << std::endl;
        flow = -std::min(bot-botMaxFill, 255-top);
    }*/

    return flow;
}

Area fluidGridOp(Area a)
{
    if (!isFluid(a.mTiles[1][1].mMat))
        return a;

    int mat00 = a.mTiles[0][0].mMat;
    int mat01 = a.mTiles[0][1].mMat;
    int mat02 = a.mTiles[0][2].mMat;
    int mat10 = a.mTiles[1][0].mMat;
    int mat11 = a.mTiles[1][1].mMat;
    int mat12 = a.mTiles[1][2].mMat;
    int mat20 = a.mTiles[2][0].mMat;
    int mat21 = a.mTiles[2][1].mMat;
    int mat22 = a.mTiles[2][2].mMat;

    int m00 = a.mTiles[0][0].mFluid;
    int m01 = a.mTiles[0][1].mFluid;
    int m02 = a.mTiles[0][2].mFluid;
    int m10 = a.mTiles[1][0].mFluid;
    int m11 = a.mTiles[1][1].mFluid;
    int m12 = a.mTiles[1][2].mFluid;
    int m20 = a.mTiles[2][0].mFluid;
    int m21 = a.mTiles[2][1].mFluid;
    int m22 = a.mTiles[2][2].mFluid;

    int o00 = a.mTiles[0][0].mSignal;
    int o01 = a.mTiles[0][1].mSignal;
    int o02 = a.mTiles[0][2].mSignal;
    int o10 = a.mTiles[1][0].mSignal;
    int o11 = a.mTiles[1][1].mSignal;
    int o12 = a.mTiles[1][2].mSignal;
    int o20 = a.mTiles[2][0].mSignal;
    int o21 = a.mTiles[2][1].mSignal;
    int o22 = a.mTiles[2][2].mSignal;

    int flow = 0;
    if (isFluid(mat21))
        flow -= vert(m11, m21, o11);
    if (isFluid(mat01))
        flow += vert(m01, m11, o01);

    // left
    if (isFluid(mat10) && m11+flow > 0)
    {
        int leftVertFlow = 0;
        if (isFluid(mat20))
            leftVertFlow -= vert(m10, m20, o10);
        if (isFluid(mat00))
            leftVertFlow += vert(m00, m10, o00);

        // Equalize the amount of water in this block and it's neighbour
        flow += int( m10 + leftVertFlow - (m11 + flow) ) / 4;
    }

    // left
    if (isFluid(mat12) && m11+flow > 0)
    {
        int rightVertFlow = 0;
        if (isFluid(mat22))
            rightVertFlow -= vert(m12, m22, o12);
        if (isFluid(mat02))
            rightVertFlow += vert(m02, m12, o02);

        // Equalize the amount of water in this block and it's neighbour
        flow += int( m12 + rightVertFlow - (m11 + flow) ) / 4;
    }

    a.mTiles[1][1].mFluid += flow;

    if (a.mTiles[1][1].mFluid > 0)
        a.mTiles[1][1].mMat = 4;
    else
        a.mTiles[1][1].mMat = 0;

    if (a.mTiles[1][1].mMat == 4)
    {
        if (a.mTiles[0][1].mMat != 4 && a.mTiles[0][1].mFluid > compress*2)
        {
            a.mTiles[1][1].mSignal = 0;
            a.mChanged = true;
        }
        else if (a.mTiles[1][1].mSignal != a.mTiles[0][1].mSignal+1)
        {
            a.mTiles[1][1].mSignal = a.mTiles[0][1].mSignal+1;
            a.mChanged = true;
        }
    }
    else
        a.mTiles[1][1].mSignal = 0;

    a.mChanged = a.mChanged || flow!=0;

    return a;

    // Flow down
    if (a.mTiles[0][1].mMat == 4 && a.mTiles[0][1].mFluid > 0 && a.mTiles[1][1].mFluid < MAX_FLUID)
    {
        int tofill = std::min<int>(a.mTiles[0][1].mFluid, MAX_FLUID-a.mTiles[1][1].mFluid);
        int change = tofill/3 + (tofill%3);
        m11 += change;
        m01 -= change;
        a.mChanged = true;
    }
    if (a.mTiles[1][1].mMat == 4 && (a.mTiles[2][1].mMat == 4 || a.mTiles[2][1].mMat == 0) &&
        a.mTiles[1][1].mFluid > 0 && a.mTiles[2][1].mFluid < MAX_FLUID)
    {
        int tofill = std::min<int>(a.mTiles[1][1].mFluid, MAX_FLUID-a.mTiles[2][1].mFluid);
        int change = tofill/3 + (tofill%3);
        m11 -= change;
        m21 += change;
        a.mChanged = true;
    }

    // Flow right to left
    if (a.mTiles[1][2].mMat == 4 && a.mTiles[1][2].mFluid > 0 && a.mTiles[1][1].mFluid < MAX_FLUID)
    {
        int tofill = std::min<int>(a.mTiles[1][2].mFluid, MAX_FLUID-a.mTiles[1][1].mFluid);
        int change = tofill/3;
        m11 += change;
        a.mChanged = true;
    }
    if (a.mTiles[1][1].mMat == 4 && (a.mTiles[1][0].mMat == 4 || a.mTiles[1][0].mMat == 0) &&
        a.mTiles[1][1].mFluid > 0 && a.mTiles[1][0].mFluid < MAX_FLUID)
    {
        int tofill = std::min<int>(a.mTiles[1][1].mFluid, MAX_FLUID-a.mTiles[1][0].mFluid);
        int change = tofill/3;
        m11 -= change;
        a.mChanged = true;
    }

    // Flow left to right
    if (a.mTiles[1][0].mMat == 4 && a.mTiles[1][0].mFluid > 0 && a.mTiles[1][1].mFluid < MAX_FLUID)
    {
        int tofill = std::min<int>(a.mTiles[1][0].mFluid, MAX_FLUID-a.mTiles[1][1].mFluid);
        int change = tofill/3;
        m11 += change;
        a.mChanged = true;
    }
    if (a.mTiles[1][1].mMat == 4 && (a.mTiles[1][2].mMat == 4 || a.mTiles[1][2].mMat == 0) &&
        a.mTiles[1][1].mFluid > 0 && a.mTiles[1][2].mFluid < MAX_FLUID)
    {
        int tofill = std::min<int>(a.mTiles[1][1].mFluid, MAX_FLUID-a.mTiles[1][2].mFluid);
        int change = tofill/3;
        m11 -= change;
        a.mChanged = true;
    }

    a.mTiles[1][1].mFluid = m11;

    if (a.mTiles[1][1].mFluid > 0 && a.mTiles[1][1].mMat == 0)
        a.mTiles[1][1].mMat = 4;
    else if (a.mTiles[1][1].mFluid == 0 && a.mTiles[1][1].mMat == 4)
        a.mTiles[1][1].mMat = 0;

    return a;
}
