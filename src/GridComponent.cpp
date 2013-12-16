#include "GridComponent.h"

#include <set>
#include <cmath>
#include <iostream>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <Fission/Core/Math.h>
#include <Fission/Core/Entity.h>
#include <Fission/Rendering/TransformComponent.h>

#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "BackGridComponent.h"
#include "FrontGridComponent.h"
#include "PlaceableComponent.h"
#include "TetraCollision.h"

int randStateCovered(int x, int y);
int randStateTop(int x, int y);
int randStateBot(int x, int y);
int randStateRight(int x, int y);
int randStateLeft(int x, int y);

TypeBits GridComponent::Type;

GridComponent::GridComponent(TransformComponent* transform, int sizeX, int sizeY, bool wrapX, Tile** tiles, int tickCount) :
    mSizeX(sizeX), mSizeY(sizeY), mWrapX(wrapX), mTiles(tiles), mTickCount(tickCount)
{
    if (transform)
        mTransformID = transform->getID();
    else
        mTransformID = -1;

    //myTransform->setOrigin(sf::Vector2f(mSizeX*TILE_SIZE, mSizeY*TILE_SIZE)/2.f);

    mCTiles.resize(tickCount);

    for (int y = 0; y < mSizeY; y++)
    {
        for (int x = 0; x < mSizeX; x++)
        {
            if (mTiles[y][x].mBack == 0)
                applyLightRec(x, y, 30);
        }
    }
}

GridComponent::~GridComponent()
{
    //dtor
}

void GridComponent::serialize(sf::Packet &packet)
{
    packet << mSizeX;
    packet << mSizeY;
    packet << mWrapX;

    for (int y = 0; y < mSizeY; y++)
        for (int x = 0; x < mSizeX; x++)
            mTiles[y][x].serialize(packet);

    packet << mTransformID;
    packet << mTickCount;
    for (int i = 0; i < mTickCount; i++)
    {
        packet << (int)mCTiles[i].size();
        for (auto& cTile : mCTiles[i])
            packet << cTile.x << cTile.y;
    }

    packet << (int)mPlaceables.size();
    for (auto placeableID : mPlaceables)
        packet << placeableID;

    mPolyShape.serialize(packet);
}

void GridComponent::deserialize(sf::Packet &packet)
{
    packet >> mSizeX;
    packet >> mSizeY;
    packet >> mWrapX;

    mTiles = new Tile*[mSizeY];

    for (int y = 0; y < mSizeY; y++)
    {
        mTiles[y] = new Tile[mSizeX];
        for (int x = 0; x < mSizeX; x++)
            mTiles[y][x].deserialize(packet);
    }


    packet >> mTransformID;
    packet >> mTickCount;
    mCTiles.resize(mTickCount);
    for (int i = 0; i < mTickCount; i++)
    {
        int cTileSize;
        packet >> cTileSize;

        mCTiles[i].resize(cTileSize);
        for (auto& cTile : mCTiles[i])
            packet >> cTile.x >> cTile.y;
    }

    int placeableCount;
    packet >> placeableCount;
    mPlaceables.resize(placeableCount);
    for (int i = 0; i < placeableCount; i++)
    {
        packet >> mPlaceables[i];
    }

    mPolyShape.deserialize(packet);
}

void GridComponent::sliceInto(Entity* newGrid, int left, int top, int right, int bot)
{
    TransformComponent* myTransform = reinterpret_cast<TransformComponent*>(Component::get(mTransformID));
    if (!myTransform)
        return;

    int width = right-left+1;
    int height = bot-top+1;

    std::cout << width << " " << height << std::endl;

    std::set<Entity*> placeables;

    Tile** tiles = new Tile*[height];
    for (int y = 0; y < height; y++)
    {
        tiles[y] = new Tile[width];
        for (int x = 0; x < width; x++)
        {
            tiles[y][x] = mTiles[top+y][left+x];
            mTiles[top+y][left+x] = Tile(); // Clear the old tile
            Entity* placeable = getPlaceableAt(left+x, top+y);
            if (placeable)
                placeables.insert(placeable);
        }
    }

    sf::Vector2f pos(left, top);
    pos *= float(TILE_SIZE);
    pos = myTransform->getTransform().transformPoint(pos);

    newGrid->giveID();
    TransformComponent* transform = new TransformComponent(pos,
                                                           myTransform->getRotation(), myTransform->getScale());
    newGrid->addComponent(transform);


    GridComponent* grid = new GridComponent(transform, width, height, false, tiles, mTickCount);
    for (Entity* placeable : placeables)
    {
        PlaceableComponent* placeableComp = reinterpret_cast<PlaceableComponent*>(placeable->getComponent(PlaceableComponent::Type));
        placeableComp->setGrid(newGrid);
        placeableComp->setGridPos(placeableComp->getGridX()-left, placeableComp->getGridY()-top);

        grid->addPlaceable(placeable);
        removePlaceable(placeable);
    }
    newGrid->addComponent(grid);

    newGrid->addComponent(new BackGridComponent(grid));
    newGrid->addComponent(new FrontGridComponent(grid));
    newGrid->addComponent(new PhysicsComponent(grid));

    for (int y = 0; y < height-1; y++)
    {
        for (int x = 0; x < width-1; x++)
        {
            if (grid->mTiles[y][x].mMat != 0 || grid->mTiles[y][x+1].mMat != 0 ||
                grid->mTiles[y+1][x+1].mMat != 0 || grid->mTiles[y+1][x].mMat != 0)
            {
                grid->mVertices.push_back(sf::Vector2f(x+1, y+1));
            }
        }
    }

    grid->recalculatePolygon();
}

void GridComponent::postDeserialize()
{
}

sf::Vector2f GridComponent::getTilePos(sf::Vector2f pos)
{
    TransformComponent* myTransform = reinterpret_cast<TransformComponent*>(Component::get(mTransformID));
    if (!myTransform)
        return sf::Vector2f();

	sf::Transform myInv = myTransform->getTransform().getInverse();
	pos = myInv.transformPoint(pos);
	pos = pos/float(TILE_SIZE);
	pos.x = floor(pos.x);
	pos.y = floor(pos.y);
	return pos;
}

void GridComponent::interact(int x, int y)
{
    if (mWrapX)
        x = wrapX(x);
    if (y < 0 || y >= mSizeY || x < 0 || x >= mSizeX)
		return;

    Entity *entity = getPlaceableAt(x, y);
    if (entity)
    {
        PlaceableComponent* placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));
        placeable->interact();
    }
}

bool GridComponent::placeMid(int x, int y, int mat)
{
    if (mWrapX)
        x = wrapX(x);
    if (y < 0 || y >= mSizeY || x < 0 || x >= mSizeX)
		return false;

    if (mTiles[y][x].mMat == mat)
        return false;

    if (!canPlace(x, y, 1, 1))
        return false;

    Tile tile = getTile(x, y);
    tile.mMat = mat;
    setTile(x, y, tile, -1);

    return true;
}

bool GridComponent::placeBack(int x, int y, int mat)
{
    if (mWrapX)
        x = wrapX(x);
    if (y < 0 || y >= mSizeY || x < 0 || x >= mSizeX)
		return false;

    if (mTiles[y][x].mBack == mat)
        return false;

    if (mTiles[y][x].mBack != 0)
        return false;

    Tile tile = getTile(x, y);
    tile.mBack = mat;
    setTile(x, y, tile, -1);

    return true;
}

bool GridComponent::addFluid(int x, int y, int mat, float fluid)
{
    if (mWrapX)
        x = wrapX(x);
    if (y < 0 || y >= mSizeY || x < 0 || x >= mSizeX)
		return false;

	mTiles[y][x].mFluid += fluid;

	return true;
}

void GridComponent::applyLightRec(int x, int y, int lastLight)
{
    if (mWrapX)
        x = wrapX(x);
    if (y < 0 || y >= mSizeY || x < 0 || x >= mSizeX)
		return;

    int blockAmount = 1;
    if (mTiles[y][x].mMat != 0)
        blockAmount = 4;

    int newLight = lastLight-blockAmount;
    if (newLight <= mTiles[y][x].mLight) return;

    mTiles[y][x].mLight = newLight;

    applyLightRec(x+1, y, newLight);
    applyLightRec(x, y+1, newLight);
    applyLightRec(x-1, y, newLight);
    applyLightRec(x, y-1, newLight);
}

void GridComponent::setTile(int x, int y, Tile tile, int tick)
{
    if (mWrapX)
        x = wrapX(x);
    if (y < 0 || y >= mSizeY || x < 0 || x >= mSizeX)
		return;

	if (mTiles[y][x].mMat == tile.mMat && mTiles[y][x].mBack == tile.mBack && mTiles[y][x].mFluid == tile.mFluid && mTiles[y][x].mLight == tile.mLight)
	{
		return;
	}

	mTiles[y][x] = tile;

	int left = wrapX(x - 1);
	int right = wrapX(x + 1);
	int top = y-1;
	int bot = y+1;

	if (top < 0)
		top = 0;
	else if (bot > mSizeY-1)
		bot = mSizeY-1;


	// If tick is -1, make it interesting for all ticks
	int tickMin = 0;
	int tickMax = mTickCount;
	if (tick != -1)
    {
		tickMin = tick;
		tickMax = tick+1;
	}

	for (int t = tickMin; t < tickMax; t++)
    {
        if (y > 0)
            setInteresting(x, y-1, t);
        setInteresting(left, y, t);
        setInteresting(x, y, t);
        setInteresting(right, y, t);
        if (y < mSizeY-1)
            setInteresting(x, y+1, t);
	}

    if (!mWrapX)
    {
        if (mTiles[y][x].mMat != 0)
        {
            bool foundTopLeft = false;
            bool foundTopRight = false;
            bool foundBotLeft = false;
            bool foundBotRight = false;
            for (unsigned int i = 0; i < mVertices.size(); i++)
            {
                if (mVertices[i] == sf::Vector2f(x, y))
                    foundTopLeft = true;
                else if (mVertices[i] == sf::Vector2f(x+1, y))
                    foundTopRight = true;
                else if (mVertices[i] == sf::Vector2f(x+1, y+1))
                    foundBotRight = true;
                else if (mVertices[i] == sf::Vector2f(x, y+1))
                    foundBotLeft = true;
            }

            if (!foundTopLeft)
                mVertices.push_back(sf::Vector2f(x, y));
            if (!foundTopRight)
                mVertices.push_back(sf::Vector2f(x+1, y));
            if (!foundBotRight)
                mVertices.push_back(sf::Vector2f(x+1, y+1));
            if (!foundBotLeft)
                mVertices.push_back(sf::Vector2f(x, y+1));
        }
        else
        {
            for (unsigned int i = 0; i < mVertices.size(); i++)
            {
                if (mVertices[i] == sf::Vector2f(x, y) || mVertices[i] == sf::Vector2f(x+1, y) ||
                    mVertices[i] == sf::Vector2f(x+1, y+1) || mVertices[i] == sf::Vector2f(x, y+1))
                {
                    mVertices.erase(mVertices.begin()+i);
                    i--;
                    continue;
                }
            }
        }

        recalculatePolygon();
    }

}

Tile GridComponent::getTile(int x, int y) const
{
    if (mWrapX)
        x = wrapX(x);
    if (y < 0 || y >= mSizeY || x < 0 || x >= mSizeX)
		return Tile();

    return mTiles[y][x];
}

bool GridComponent::canPlace(int x, int y, int width, int height)
{
    if (mWrapX)
        x = wrapX(x);
    if (y < 0 || y >= mSizeY || x < 0 || x >= mSizeX)
		return false;

    for (int i = y; i < y+height; i++)
    {
        for (int j = x; j < x+width; j++)
        {
            if (mTiles[i][j].mMat != 0)
                return false;
        }
    }

    for (auto placeableID : mPlaceables)
    {
        Entity* entity = Entity::get(placeableID);
        PlaceableComponent* placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));
        if (x+width > placeable->getGridX() && x < placeable->getGridX()+placeable->getWidth() &&
            y+height > placeable->getGridY() && y < placeable->getGridY()+placeable->getHeight())
            return false;
    }

    return true;
}

void GridComponent::addPlaceable(Entity* entity)
{
    TransformComponent* myTransform = reinterpret_cast<TransformComponent*>(Component::get(mTransformID));
    if (!myTransform)
        return;

    entity->giveID();

    TransformComponent* trans = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    PlaceableComponent* placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));

    if (mWrapX)
        placeable->setGridX(wrapX(placeable->getGridX()));

    sf::Vector2f pos(placeable->getGridX(), placeable->getGridY());
    pos *= float(TILE_SIZE);
    pos = myTransform->getTransform().transformPoint(pos);
    trans->setPosition(pos);
    trans->setRotation(myTransform->getRotation());
    trans->setScale(myTransform->getScale());
    mPlaceables.push_back(entity->getID());
}

void GridComponent::removePlaceable(Entity* placeable)
{
    for (unsigned int i = 0; i < mPlaceables.size(); i++)
    {
        if (Entity::get(mPlaceables[i]) == placeable)
        {
            mPlaceables.erase(mPlaceables.begin()+i);
            return;
        }
    }
}

Entity* GridComponent::getPlaceableAt(int x, int y)
{
    if (mWrapX)
        x = wrapX(x);
    else if (x < 0 || x >= mSizeX-1)
        return NULL;

    for (auto placeableID : mPlaceables)
    {
        Entity* entity = Entity::get(placeableID);
        PlaceableComponent* placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));
        if (x >= placeable->getGridX() && x < placeable->getGridX()+placeable->getWidth() &&
            y >= placeable->getGridY() && y < placeable->getGridY()+placeable->getHeight())
            return entity;
    }

    return NULL;
}

void GridComponent::recalculatePolygon()
{
    sf::Vector2f centroid;
    for (auto& vert : mVertices)
        centroid += vert;
    centroid /= static_cast<float>(mVertices.size());

    std::vector<sf::Vector2f> vertices;

    for (unsigned int i = 0; i < mVertices.size(); i++)
    {
        sf::Vector2f dir = normalize(mVertices[i]-centroid);
        sf::Vector2f support = calcSupportPoint(mVertices.data(), mVertices.size(), dir);

        if (equal(mVertices[i].x, support.x) && equal(mVertices[i].y, support.y))
        {
            vertices.push_back(mVertices[i]);
        }
    }

    std::cout << vertices.size() << std::endl;

    if (vertices.size() > 2)
        mPolyShape.set(vertices.data(), vertices.size());
}

int GridComponent::calcNeighborStateBack(int x, int y)
{
    x = wrapX(x);
	int left = x - 1;
	int right = x + 1;
	if (mWrapX)
    {
        left = wrapX(left);
        right = wrapX(right);
    }

    if (mTiles[y][x].mBack == 0 || mTiles[y][x].mBack == 4)
        return 0;

	Tile a[3][3]; // Area

	if (y > 0)
    {
        if (left >= 0 && left < mSizeX)
            a[0][0] = mTiles[y-1][left];
		a[0][1] = mTiles[y-1][x];
		if (right >= 0 && right < mSizeX)
            a[0][2] = mTiles[y-1][right];
	}

    if (left >= 0 && left < mSizeX)
        a[1][0] = mTiles[y][left];
	a[1][1] = mTiles[y][x];
	if (right >= 0 && right < mSizeX)
        a[1][2] = mTiles[y][right];

	if (y < mSizeY-1)
    {
        if (left >= 0 && left < mSizeX)
            a[2][0] = mTiles[y+1][left];
		a[2][1] = mTiles[y+1][x];
		if (right >= 0 && right < mSizeX)
            a[2][2] = mTiles[y+1][right];
	}

	if (a[0][1].mBack == a[1][1].mBack &&
		a[1][0].mBack == a[1][1].mBack && a[1][2].mBack == a[1][1].mBack &&
		a[2][1].mBack == a[1][1].mBack)
		return randStateCovered(x, y);
	else if (a[0][1].mBack != a[1][1].mBack &&
		a[1][0].mBack != a[1][1].mBack && a[1][2].mBack != a[1][1].mBack &&
		a[2][1].mBack != a[1][1].mBack)
		return 22;
	else if (a[0][1].mBack != a[1][1].mBack &&
		a[1][0].mBack == a[1][1].mBack && a[1][2].mBack == a[1][1].mBack &&
		a[2][1].mBack == a[1][1].mBack)
		return randStateTop(x, y);
	else if (a[0][1].mBack == a[1][1].mBack &&
		a[1][0].mBack == a[1][1].mBack && a[1][2].mBack == a[1][1].mBack &&
		a[2][1].mBack != a[1][1].mBack)
		return randStateBot(x, y);
	else if (a[0][1].mBack == a[1][1].mBack &&
		a[1][0].mBack != a[1][1].mBack && a[1][2].mBack == a[1][1].mBack &&
		a[2][1].mBack == a[1][1].mBack)
		return randStateLeft(x, y);
	else if (a[0][1].mBack == a[1][1].mBack &&
		a[1][0].mBack == a[1][1].mBack && a[1][2].mBack != a[1][1].mBack &&
		a[2][1].mBack == a[1][1].mBack)
		return randStateRight(x, y);
	else if (a[0][1].mBack != a[1][1].mBack &&
		a[1][0].mBack == a[1][1].mBack && a[1][2].mBack == a[1][1].mBack &&
		a[2][1].mBack != a[1][1].mBack)
		return 0;
	else if (a[0][1].mBack == a[1][1].mBack &&
		a[1][0].mBack != a[1][1].mBack && a[1][2].mBack != a[1][1].mBack &&
		a[2][1].mBack == a[1][1].mBack)
		return 1;
	else if (a[0][1].mBack != a[1][1].mBack &&
		a[1][0].mBack != a[1][1].mBack && a[1][2].mBack == a[1][1].mBack &&
		a[2][1].mBack == a[1][1].mBack)
		return 18;
	else if (a[0][1].mBack != a[1][1].mBack &&
		a[1][0].mBack == a[1][1].mBack && a[1][2].mBack != a[1][1].mBack &&
		a[2][1].mBack == a[1][1].mBack)
		return 21;
	else if (a[0][1].mBack == a[1][1].mBack &&
		a[1][0].mBack != a[1][1].mBack && a[1][2].mBack == a[1][1].mBack &&
		a[2][1].mBack != a[1][1].mBack)
		return 20;
	else if (a[0][1].mBack == a[1][1].mBack &&
		a[1][0].mBack == a[1][1].mBack && a[1][2].mBack != a[1][1].mBack &&
		a[2][1].mBack != a[1][1].mBack)
		return 19;
	else if (a[0][1].mBack != a[1][1].mBack &&
		a[1][0].mBack != a[1][1].mBack && a[1][2].mBack != a[1][1].mBack &&
		a[2][1].mBack == a[1][1].mBack)
		return 2;
	else if (a[0][1].mBack != a[1][1].mBack &&
		a[1][0].mBack == a[1][1].mBack && a[1][2].mBack != a[1][1].mBack &&
		a[2][1].mBack != a[1][1].mBack)
		return 3;
	else if (a[0][1].mBack == a[1][1].mBack &&
		a[1][0].mBack != a[1][1].mBack && a[1][2].mBack != a[1][1].mBack &&
		a[2][1].mBack != a[1][1].mBack)
		return 4;
	else if (a[0][1].mBack != a[1][1].mBack &&
		a[1][0].mBack != a[1][1].mBack && a[1][2].mBack == a[1][1].mBack &&
		a[2][1].mBack != a[1][1].mBack)
		return 5;
}

int GridComponent::calcNeighborStateMid(int x, int y)
{
    x = wrapX(x);
	int left = x - 1;
	int right = x + 1;
	if (mWrapX)
    {
        left = wrapX(left);
        right = wrapX(right);
    }

    if (mTiles[y][x].mMat == 0 || mTiles[y][x].mMat == 4)
        return 0;

	Tile a[3][3]; // Area

	if (y > 0)
    {
        if (left >= 0 && left < mSizeX)
            a[0][0] = mTiles[y-1][left];
		a[0][1] = mTiles[y-1][x];
		if (right >= 0 && right < mSizeX)
            a[0][2] = mTiles[y-1][right];
	}

    if (left >= 0 && left < mSizeX)
        a[1][0] = mTiles[y][left];
	a[1][1] = mTiles[y][x];
	if (right >= 0 && right < mSizeX)
        a[1][2] = mTiles[y][right];

	if (y < mSizeY-1)
    {
        if (left >= 0 && left < mSizeX)
            a[2][0] = mTiles[y+1][left];
		a[2][1] = mTiles[y+1][x];
		if (right >= 0 && right < mSizeX)
            a[2][2] = mTiles[y+1][right];
	}

	if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return randStateCovered(x, y);
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 22;
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return randStateTop(x, y);
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return randStateBot(x, y);
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return randStateLeft(x, y);
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return randStateRight(x, y);
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 0;
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return 1;
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return 18;
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return 21;
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 20;
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 19;
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return 2;
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 3;
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 4;
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 5;
}

int GridComponent::wrapX(int x) const
{
    if (x < 0)
    {
		x = x % mSizeX;
		if (x < 0)
			x += mSizeX;
	}
	else if (x >= mSizeX)
		x = x % mSizeX;

	return x;
}

Area GridComponent::getArea(int x, int y) const
{
    int left = wrapX(x - 1);
    int right = wrapX(x + 1);

    // Tiles
    Area a;
    a.mX = x;
    a.mY = y;
    if (y > 0)
    {
        a.mTiles[0][0] = mTiles[y-1][left];
        a.mTiles[0][1] = mTiles[y-1][x];
        a.mTiles[0][2] = mTiles[y-1][right];
    }

    a.mTiles[1][0] = mTiles[y][left];
    a.mTiles[1][1] = mTiles[y][x];
    a.mTiles[1][2] = mTiles[y][right];

    if (y < mSizeY-1)
    {
        a.mTiles[2][0] = mTiles[y+1][left];
        a.mTiles[2][1] = mTiles[y+1][x];
        a.mTiles[2][2] = mTiles[y+1][right];
    }

    return a;
}

// ****************************************************************************

int random(int x, int y, int min, int max)
{
    return ((x ^ y)%(max-min+1))+min;
}

int randStateCovered(int x, int y)
{
	return random(x, y, 6, 9);
}

int randStateTop(int x, int y)
{
	return random(x, y, 10, 11);
}

int randStateBot(int x, int y)
{
	return random(x, y, 12, 13);
}

int randStateRight(int x, int y)
{
	return random(x, y, 14, 15);
}

int randStateLeft(int x, int y)
{
	return random(x, y, 16, 17);
}
