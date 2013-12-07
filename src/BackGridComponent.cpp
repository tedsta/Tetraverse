#include "BackGridComponent.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "GridComponent.h"

TypeBits BackGridComponent::Type;

BackGridComponent::BackGridComponent(GridComponent* grid)
{
    if (grid)
        mGridID = grid->getID();
    else
        mGridID = -1;
}

BackGridComponent::~BackGridComponent()
{
    //dtor
}

void BackGridComponent::serialize(sf::Packet &packet)
{
    packet << mGridID;
}

void BackGridComponent::deserialize(sf::Packet &packet)
{
    packet >> mGridID;
}

void BackGridComponent::render(sf::RenderTarget& target, sf::RenderStates states)
{
    GridComponent* grid = reinterpret_cast<GridComponent*>(Component::get(mGridID));
    if (!grid)
        return;

    float tsize = static_cast<float>(TILE_SIZE);

	sf::Vector2f center = states.transform.transformPoint(sf::Vector2f());
	center -= target.getView().getCenter();
	center += sf::Vector2f(target.getView().getSize().x, target.getView().getSize().y)/2.f;
	center.x *= -1;
	center.y *= -1;

	sf::Vector2f centerT = center/tsize;                // Center tiled
	float ssXT = ceil(target.getView().getSize().x / tsize); // Screen size X in tiles
	float ssYT = ceil(target.getView().getSize().y / tsize); // Screen size Y in tiles

	int left;
	int right;
	if (grid->mWrapX)
    {
        left = centerT.x - 1;
        right = centerT.x + ssXT + 1;
    }
    else
    {
        left = std::max<int>(centerT.x-1, 0);
        right = std::min<int>(centerT.x+ssXT+1, grid->mSizeX-1);
    }

	int top = std::max<int>(centerT.y-1, 0);
	int bot = std::min<int>(centerT.y+ssYT+1, grid->mSizeY-1);

    sf::VertexArray verts(sf::Quads, 4);
    sf::VertexArray outline(sf::LinesStrip, 5);
	for (int _y = top; _y <= bot; _y++)
	{
		for (int _x = left; _x <= right; _x++)
        {
			int x = grid->wrapX(_x);
			int y = _y;

			auto start = sf::Vector2f(tsize * static_cast<float>(_x), tsize * static_cast<float>(_y)); // Tile start draw
			verts[0] = sf::Vertex(start,
				sf::Color::White,
				sf::Vector2f());
			verts[1] = sf::Vertex(start+sf::Vector2f(0, tsize),
				sf::Color::White,
				sf::Vector2f(0, tsize));
			verts[2] = sf::Vertex(start+sf::Vector2f(tsize, tsize),
				sf::Color::White,
				sf::Vector2f(tsize, tsize));
			verts[3] = sf::Vertex(start+sf::Vector2f(tsize, 0),
				sf::Color::White,
				sf::Vector2f(tsize, 0));
			outline[0] = sf::Vertex(start,
				sf::Color::White,
				sf::Vector2f());
			outline[1] = sf::Vertex(start+sf::Vector2f(0, tsize),
				sf::Color::White,
				sf::Vector2f(0, tsize));
			outline[2] = sf::Vertex(start+sf::Vector2f(tsize, tsize),
				sf::Color::White,
				sf::Vector2f(tsize, tsize));
			outline[3] = sf::Vertex(start+sf::Vector2f(tsize, 0),
				sf::Color::White,
				sf::Vector2f(tsize, 0));
            outline[4] = sf::Vertex(start,
				sf::Color::White,
				sf::Vector2f());

            if (grid->mTiles[y][x].mFluid > 0)
            {
                states.texture = NULL;
                verts[0].color = sf::Color(0, 0, 255, std::min(grid->mTiles[y][x].mFluid*128.f, 255.f));
                verts[1].color = sf::Color(0, 0, 255, std::min(grid->mTiles[y][x].mFluid*128.f, 255.f));
                verts[2].color = sf::Color(0, 0, 255, std::min(grid->mTiles[y][x].mFluid*128.f, 255.f));
                verts[3].color = sf::Color(0, 0, 255, std::min(grid->mTiles[y][x].mFluid*128.f, 255.f));
                target.draw(verts, states);
            }
            else
            {
                if (grid->mTiles[y][x].mBack == 0 || grid->mTiles[y][x].mBack >= GridComponent::TileSheets.size())
                    continue;

                // Grab tile sheet info
                sf::Texture* sheet = GridComponent::TileSheets[grid->mTiles[y][x].mBack];
                if (!sheet)
                    continue;

                int sheetSizeX = sheet->getSize().x / TILE_SIZE;
                int sheetSizeY = sheet->getSize().y / TILE_SIZE;

                int edgeState = grid->calcNeighborState(x, y);
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
		}
	}
}
