#include "GridComponent.h"

#include <cmath>
#include <iostream>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <Fission/Core/Entity.h>
#include <Fission/Rendering/TransformComponent.h>

#include "PlaceableComponent.h"

int randStateCovered(int x, int y);
int randStateTop(int x, int y);
int randStateBot(int x, int y);
int randStateRight(int x, int y);
int randStateLeft(int x, int y);

TypeBits GridComponent::Type;
std::vector<sf::Texture*> GridComponent::TileSheets;

GridComponent::GridComponent(sf::Transformable* transform, int sizeX, int sizeY, Tile** tiles, int tickCount) :
    mTransform(transform), mSizeX(sizeX), mSizeY(sizeY), mTiles(tiles), mTickCount(tickCount)
{
	for (int y = 0; y < mSizeY; y++)
		for (int x = 0; x < mSizeX; x++)
			calcNeighborState(x, y);

    mCTiles.resize(tickCount);
}

GridComponent::~GridComponent()
{
    //dtor
}

void GridComponent::serialize(sf::Packet &packet)
{
}

void GridComponent::deserialize(sf::Packet &packet)
{
}

void GridComponent::render(sf::RenderTarget& target, sf::RenderStates states)
{
    float tsize = static_cast<float>(TILE_SIZE);

	sf::Vector2f center = states.transform.transformPoint(sf::Vector2f());
	center -= target.getView().getCenter();
	center += sf::Vector2f(target.getSize().x, target.getSize().y)/2.f;
	center.x *= -1;
	center.y *= -1;

	sf::Vector2f centerT = center/tsize;                // Center tiled
	float ssXT = ceil(target.getSize().x / tsize); // Screen size X in tiles
	float ssYT = ceil(target.getSize().y / tsize); // Screen size Y in tiles

	int left = centerT.x - 1;
	int top = std::max<int>(centerT.y-1, 0);
	int right = centerT.x + ssXT + 1;
	int bot = std::min<int>(centerT.y+ssYT+1, mSizeY);

    sf::VertexArray verts(sf::Quads, 4);
	for (int _y = top; _y <= bot; _y++)
	{
		for (int _x = left; _x <= right; _x++)
        {
			int x = wrapX(_x);
			int y = _y;

			auto start = sf::Vector2f(tsize * static_cast<float>(_x), tsize * static_cast<float>(_y)); // Tile start draw
			verts[0] = sf::Vertex(start,
				sf::Color(255, 255, 255, 255),
				sf::Vector2f());
			verts[1] = sf::Vertex(start+sf::Vector2f(0, tsize),
				sf::Color(255, 255, 255, 255),
				sf::Vector2f(0, tsize));
			verts[2] = sf::Vertex(start+sf::Vector2f(tsize, tsize),
				sf::Color(255, 255, 255, 255),
				sf::Vector2f(tsize, tsize));
			verts[3] = sf::Vertex(start+sf::Vector2f(tsize, 0),
				sf::Color(255, 255, 255, 255),
				sf::Vector2f(tsize, 0));

            if (mTiles[y][x].mFluid > 0)
            {
                states.texture = NULL;
                verts[0].color = sf::Color(255, 0, 0, std::min(mTiles[y][x].mFluid*128.f, 255.f));
                verts[1].color = sf::Color(255, 0, 0, std::min(mTiles[y][x].mFluid*128.f, 255.f));
                verts[2].color = sf::Color(255, 0, 0, std::min(mTiles[y][x].mFluid*128.f, 255.f));
                verts[3].color = sf::Color(255, 0, 0, std::min(mTiles[y][x].mFluid*128.f, 255.f));
                target.draw(verts, states);
            }
            else
            {
                if (mTiles[y][x].mWire > 0)
                {
                    states.texture = NULL;
                    verts[0].color = sf::Color(mTiles[y][x].mWire, mTiles[y][x].mWire, mTiles[y][x].mSignal, 255);
                    verts[1].color = sf::Color(mTiles[y][x].mWire, mTiles[y][x].mWire, mTiles[y][x].mSignal, 255);
                    verts[2].color = sf::Color(mTiles[y][x].mWire, mTiles[y][x].mWire, mTiles[y][x].mSignal, 255);
                    verts[3].color = sf::Color(mTiles[y][x].mWire, mTiles[y][x].mWire, mTiles[y][x].mSignal, 255);

                    target.draw(verts, states);
                }


                if (mTiles[y][x].mMat == 0 || mTiles[y][x].mMat >= TileSheets.size())
                    continue;

                // Grab tile sheet info
                sf::Texture* sheet = TileSheets[mTiles[y][x].mMat];
                if (!sheet)
                    continue;

                int sheetSizeX = sheet->getSize().x / TILE_SIZE;
                int sheetSizeY = sheet->getSize().y / TILE_SIZE;

                int edgeState = calcNeighborState(x, y);
                float texStartX = float(edgeState%sheetSizeX) * tsize;
                float texStartY = float(edgeState/sheetSizeY) * tsize;

                verts[0].texCoords.x += texStartX;
                verts[0].texCoords.y += texStartY;
                verts[1].texCoords.x += texStartX;
                verts[1].texCoords.y += texStartY;
                verts[2].texCoords.x += texStartX;
                verts[2].texCoords.y += texStartY;
                verts[3].texCoords.x += texStartX;
                verts[3].texCoords.y += texStartY;

                states.texture = sheet;
                target.draw(verts, states);
            }

			/*if joelMode {
				for i, c := range mTiles[y][x].comp {
					bheight := (float32(c) / 255 * 16)
					bwidth := float32(TILE_SIZE / MaxComps)
					bstart := start.Add(sf::Vector2f{float32(i) * bwidth, 16 - bheight})

					color := sf::Color{}
					color.A = 255

					switch i + int(mTiles[y][x].mMat) {
					case 0:
						color.R = 255
					case 1:
						color.G = 255
					case 2:
						color.B = 255
					case 3:
						color.R = 255
						color.G = 255
					case 4:
						color.R = 255
						color.B = 255
					case 5:
						color.R = 255
						color.G = 255
						color.B = 255
					case 6:
						color.G = 255
						color.B = 255
					case 7:
						color.R = 255
						color.G = 255
						color.B = 255
					}

					verts[0] = sf::Vertex{bstart,
						color,
						sf::Vector2f{}}
					verts[1] = sf::Vertex{bstart.Add(sf::Vector2f{0, bheight}),
						color,
						sf::Vector2f{}}
					verts[2] = sf::Vertex{bstart.Add(sf::Vector2f{bwidth, bheight}),
						color,
						sf::Vector2f{}}
					verts[3] = sf::Vertex{bstart.Add(sf::Vector2f{bwidth, 0}),
						color,
						sf::Vector2f{}}

					states.Texture = nil
					rt.Render(verts[:], sf::Quads, states)
				}
			}*/
		}
	}
}

bool GridComponent::checkCollision(sf::Transformable* trans, sf::Vector2f dim, int dir, float& fix)
{
	sf::Transform myInv = mTransform->getTransform().getInverse(); // Inverse of grid's transform
	sf::Vector2f cOffset = myInv.transformPoint(trans->getPosition());
	sf::Vector2f offset = cOffset-(dim/2.f);
	sf::Vector2f tOffset = offset/float(TILE_SIZE);

	int x = floor(tOffset.x);
	int y = floor(tOffset.y);
	int right = ceil((offset.x+dim.x)/float(TILE_SIZE)) - 1;
	int bot = ceil((offset.y+dim.y)/float(TILE_SIZE)) - 1;

	// Let's be safe about this
	if (y >= int(mSizeY) || bot < 0)
        return 0;

	if (y < 0)
		y = 0;
	if (bot >= int(mSizeY))
		bot = int(mSizeY) - 1;

	// Test the collision
	int tfix = 0;
	bool col = dirCollision(x, y, right, bot, dir, tfix);

	if (col)
	{
		// Calculate remainder pixels depending on direction
		switch (dir)
        {
		case LEFT:
			fix = float(TILE_SIZE) - fmod(offset.x, TILE_SIZE);
			break;
		case RIGHT:
			fix = float(tfix*TILE_SIZE) - fmod(offset.x+dim.x, TILE_SIZE);
			break;
		case UP:
			fix = float(TILE_SIZE) - fmod(offset.y, TILE_SIZE);
			break;
		case DOWN:
			fix = float(tfix*TILE_SIZE) - fmod(offset.y+dim.y, TILE_SIZE);
			break;
		}
		if (offset.x <= 0 && offset.x > -dim.x && dir == RIGHT)
        {
		}
		else if (offset.x <= 0 && (dir == LEFT || dir == RIGHT))
			fix -= float(TILE_SIZE);
	}

	if (cOffset.x-(dim.x/2) <= 0 || cOffset.x+(dim.x/2) >= mSizeX*TILE_SIZE)
    {
		cOffset.x = fmod(cOffset.x, mSizeX*TILE_SIZE);
		sf::Transform t = mTransform->getTransform();
		trans->setPosition(t.transformPoint(cOffset));
	}

	return col;
}

bool GridComponent::dirCollision(int left, int top, int right, int bot, int dir, int& fix)
{
	for (int _y = top; _y <= bot; _y++)
	{
		for (int _x = left; _x <= right; _x++)
        {
			int x = wrapX(_x);
			int y = _y;

			if (mTiles[y][x].mMat != 0 && mTiles[y][x].mMat != 4)
            {
				switch (dir)
				{
				case RIGHT:
					fix = -(right - _x);
					break;
				case DOWN:
					fix = -(bot - _y);
					break;
				}
				return true;
			}
		}
	}
	return false;
}

Tile** GridComponent::slice(int left, int top, int right, int bot)
{
    Tile** tiles = new Tile*[bot-top];
    for (int y = 0; y < bot-top; y++)
    {
        tiles[y] = new Tile[right-left];
        for (int x = 0; x < right-left; x++)
        {
            tiles[y][x] = mTiles[top+y][left+x];
            mTiles[top+y][left+x] = Tile(); // Clear the old tile
        }
    }

    return tiles;
}

sf::Vector2f GridComponent::getTilePos(sf::Vector2f pos)
{
	sf::Transform myInv = mTransform->getTransform().getInverse();
	pos = myInv.transformPoint(pos);
	pos = pos/float(TILE_SIZE);
	pos.x = floor(pos.x);
	pos.y = floor(pos.y);
	return pos;
}

void GridComponent::interact(int x, int y)
{
    Entity *entity = getPlaceableAt(x, y);
    if (entity)
    {
        PlaceableComponent* placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));
        placeable->interact();
    }
}

void GridComponent::addFluid(int x, int y, float fluid)
{
    if (y < 0 || y >= mSizeY)
		return;

	x = wrapX(x);

	mTiles[y][x].mFluid += fluid;
}

void GridComponent::setTile(int x, int y, Tile tile, int tick)
{
	if (y < 0 || y >= mSizeY)
		return;

	x = wrapX(x);

	if (mTiles[y][x].mMat == tile.mMat && mTiles[y][x].mFluid == tile.mFluid &&
        mTiles[y][x].mWire == tile.mWire && mTiles[y][x].mSignal == tile.mSignal)
	{
		return;
	}

	//std::cout << int(mTiles[y][x].mMat) << std::endl;

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
}

bool GridComponent::canPlace(int x, int y, int width, int height)
{
    for (int i = y; i < y+height; i++)
    {
        for (int j = x; j < x+width; j++)
        {
            if (mTiles[i][j].mMat != 0)
                return false;
        }
    }

    for (auto entity : mPlaceables)
    {
        PlaceableComponent* placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));
        if (x+width > placeable->getGridX() && x < placeable->getGridX()+placeable->getWidth() &&
            y+height > placeable->getGridY() && y < placeable->getGridY()+placeable->getHeight())
            return false;
    }

    return true;
}

void GridComponent::addPlaceable(Entity* entity)
{
    TransformComponent* trans = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    PlaceableComponent* placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));

    sf::Vector2f pos(placeable->getGridX(), placeable->getGridY());
    pos *= float(TILE_SIZE);
    mTransform->getTransform().transformPoint(pos);
    trans->setPosition(pos);
    trans->setRotation(mTransform->getRotation());
    trans->setScale(mTransform->getScale());
    mPlaceables.push_back(entity);
}

Entity* GridComponent::getPlaceableAt(int x, int y)
{
    for (auto entity : mPlaceables)
    {
        PlaceableComponent* placeable = reinterpret_cast<PlaceableComponent*>(entity->getComponent(PlaceableComponent::Type));
        if (x >= placeable->getGridX() && x < placeable->getGridX()+placeable->getWidth() &&
            y >= placeable->getGridY() && y < placeable->getGridY()+placeable->getHeight())
            return entity;
    }

    return NULL;
}

int GridComponent::calcNeighborState(int x, int y)
{
    x = wrapX(x);
	int left = wrapX(x - 1);
	int right = wrapX(x + 1);

    if (mTiles[y][x].mMat == 0 || mTiles[y][x].mMat == 4)
        return 0;

	Tile a[3][3]; // Area

	if (y > 0)
    {
		a[0][0] = mTiles[y-1][left];
		a[0][1] = mTiles[y-1][x];
		a[0][2] = mTiles[y-1][right];
	}

	a[1][0] = mTiles[y][left];
	a[1][1] = mTiles[y][x];
	a[1][2] = mTiles[y][right];

	if (y < mSizeY-1)
    {
		a[2][0] = mTiles[y+1][left];
		a[2][1] = mTiles[y+1][x];
		a[2][2] = mTiles[y+1][right];
	}

	if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return randStateCovered(x, y);
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 21;
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
		return 17;
	else if (a[0][1].mMat != a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat == a[1][1].mMat)
		return 18;
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat != a[1][1].mMat && a[1][2].mMat == a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 19;
	else if (a[0][1].mMat == a[1][1].mMat &&
		a[1][0].mMat == a[1][1].mMat && a[1][2].mMat != a[1][1].mMat &&
		a[2][1].mMat != a[1][1].mMat)
		return 20;
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

int GridComponent::wrapX(int x)
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

Area GridComponent::getArea(int x, int y)
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
	return 16;
}
