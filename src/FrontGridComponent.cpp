#include "FrontGridComponent.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "GridComponent.h"

TypeBits FrontGridComponent::Type;

FrontGridComponent::FrontGridComponent(GridComponent* grid) : mGrid(grid)
{
    //ctor
}

FrontGridComponent::~FrontGridComponent()
{
    //dtor
}

void FrontGridComponent::render(sf::RenderTarget& target, sf::RenderStates states)
{
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
	if (mGrid->mWrapX)
    {
        left = centerT.x - 1;
        right = centerT.x + ssXT + 1;
    }
    else
    {
        left = std::max<int>(centerT.x-1, 0);
        right = std::min<int>(centerT.x+ssXT+1, mGrid->mSizeX-1);
    }

	int top = std::max<int>(centerT.y-1, 0);
	int bot = std::min<int>(centerT.y+ssYT+1, mGrid->mSizeY-1);

    sf::VertexArray verts(sf::Quads, 4);
    sf::VertexArray outline(sf::LinesStrip, 5);
	for (int _y = top; _y <= bot; _y++)
	{
		for (int _x = left; _x <= right; _x++)
        {
			int x = mGrid->wrapX(_x);
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

            if (mGrid->mTiles[y][x].mFluid > 0)
            {
                states.texture = NULL;
                verts[0].color = sf::Color(0, 0, 255, std::min(mGrid->mTiles[y][x].mFluid*128.f, 255.f));
                verts[1].color = sf::Color(0, 0, 255, std::min(mGrid->mTiles[y][x].mFluid*128.f, 255.f));
                verts[2].color = sf::Color(0, 0, 255, std::min(mGrid->mTiles[y][x].mFluid*128.f, 255.f));
                verts[3].color = sf::Color(0, 0, 255, std::min(mGrid->mTiles[y][x].mFluid*128.f, 255.f));
                target.draw(verts, states);
            }
            else
            {
                if (mGrid->mTiles[y][x].mWire > 0)
                {
                    states.texture = NULL;
                    verts[0].color = sf::Color(mGrid->mTiles[y][x].mWire, mGrid->mTiles[y][x].mWire, mGrid->mTiles[y][x].mSignal, 255);
                    verts[1].color = sf::Color(mGrid->mTiles[y][x].mWire, mGrid->mTiles[y][x].mWire, mGrid->mTiles[y][x].mSignal, 255);
                    verts[2].color = sf::Color(mGrid->mTiles[y][x].mWire, mGrid->mTiles[y][x].mWire, mGrid->mTiles[y][x].mSignal, 255);
                    verts[3].color = sf::Color(mGrid->mTiles[y][x].mWire, mGrid->mTiles[y][x].mWire, mGrid->mTiles[y][x].mSignal, 255);

                    target.draw(verts, states);
                }


                if (mGrid->mTiles[y][x].mMat == 0 || mGrid->mTiles[y][x].mMat >= GridComponent::TileSheets.size())
                    continue;

                // Grab tile sheet info
                sf::Texture* sheet = GridComponent::TileSheets[mGrid->mTiles[y][x].mMat];
                if (!sheet)
                    continue;

                int sheetSizeX = sheet->getSize().x / TILE_SIZE;
                int sheetSizeY = sheet->getSize().y / TILE_SIZE;

                int edgeState = mGrid->calcNeighborState(x, y);
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
				for i, c := range mGrid->mTiles[y][x].comp {
					bheight := (float32(c) / 255 * 16)
					bwidth := float32(TILE_SIZE / MaxComps)
					bstart := start.Add(sf::Vector2f{float32(i) * bwidth, 16 - bheight})

					color := sf::Color{}
					color.A = 255

					switch i + int(mGrid->mTiles[y][x].mMat) {
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


			for(int j = 0; j < mGrid->mCTiles.size();j++){
                for (int i = 0; i < mGrid->getInterestingTiles(j).size(); i++)
                    {
                        int u = mGrid->getInterestingTiles(j)[i].x;
                        int v = mGrid->getInterestingTiles(j)[i].y;
                        if (u == x && v == y)
                        {
                            verts.setPrimitiveType(sf::LinesStrip);
                            states.texture = NULL;
                            outline[0].color = sf::Color(255, 0, 0, 255);
                            outline[1].color = sf::Color(255, 0, 0, 255);
                            outline[2].color = sf::Color(255, 0, 0, 255);
                            outline[3].color = sf::Color(255, 0, 0, 255);
                            outline[4].color = sf::Color(255, 0, 0, 255);
                            target.draw(verts, states);
                             verts.setPrimitiveType(sf::Quads);
                        }
                }
			}
		}
	}
}

void FrontGridComponent::renderShadow(sf::RenderTarget& target, sf::RenderStates states)
{
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
	if (mGrid->mWrapX)
    {
        left = centerT.x - 1;
        right = centerT.x + ssXT + 1;
    }
    else
    {
        left = std::max<int>(centerT.x-1, 0);
        right = std::min<int>(centerT.x+ssXT+1, mGrid->mSizeX-1);
    }

	int top = std::max<int>(centerT.y-1, 0);
	int bot = std::min<int>(centerT.y+ssYT+1, mGrid->mSizeY-1);

    sf::VertexArray verts(sf::Quads, 4);
    sf::VertexArray outline(sf::LinesStrip, 5);
	for (int _y = top; _y <= bot; _y++)
	{
		for (int _x = left; _x <= right; _x++)
        {
			int x = mGrid->wrapX(_x);
			int y = _y;

			auto start = sf::Vector2f(tsize * static_cast<float>(_x), tsize * static_cast<float>(_y)); // Tile start draw
			verts[0] = sf::Vertex(start,
				sf::Color::Black,
				sf::Vector2f());
			verts[1] = sf::Vertex(start+sf::Vector2f(0, tsize),
				sf::Color::Black,
				sf::Vector2f(0, tsize));
			verts[2] = sf::Vertex(start+sf::Vector2f(tsize, tsize),
				sf::Color::Black,
				sf::Vector2f(tsize, tsize));
			verts[3] = sf::Vertex(start+sf::Vector2f(tsize, 0),
				sf::Color::Black,
				sf::Vector2f(tsize, 0));

            if (mGrid->mTiles[y][x].mMat == 0 || mGrid->mTiles[y][x].mMat >= GridComponent::TileSheets.size())
                continue;

            // Grab tile sheet info
            sf::Texture* sheet = GridComponent::TileSheets[mGrid->mTiles[y][x].mMat];
            if (!sheet)
                continue;

            int sheetSizeX = sheet->getSize().x / TILE_SIZE;
            int sheetSizeY = sheet->getSize().y / TILE_SIZE;

            int edgeState = mGrid->calcNeighborState(x, y);
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
