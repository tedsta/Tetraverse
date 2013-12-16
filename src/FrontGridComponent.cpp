#include "FrontGridComponent.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <Fission/Rendering/RenderSystem.h>
#include <Fission/Rendering/TransformComponent.h>
#include <Fission/Rendering/RenderComponent.h>

#include "GridComponent.h"
#include "TetraCollision.h"

TypeBits FrontGridComponent::Type;
RenderSystem* FrontGridComponent::RndSys;
std::vector<sf::Texture*> FrontGridComponent::TileSheets;

FrontGridComponent::FrontGridComponent(GridComponent* grid)
{
    if (grid)
        mGridID = grid->getID();
    else
        mGridID = -1;

    setLayer(4);
}

FrontGridComponent::~FrontGridComponent()
{
    //dtor
}

void FrontGridComponent::serialize(sf::Packet &packet)
{
    RenderComponent::serialize(packet);

    packet << mGridID;
}

void FrontGridComponent::deserialize(sf::Packet &packet)
{
    RenderComponent::deserialize(packet);

    packet >> mGridID;
}

void FrontGridComponent::postDeserialize()
{
}

void FrontGridComponent::render(sf::RenderTarget& target, sf::RenderStates states)
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

    /*if (grid->mWrapX)
    {
        for (auto rEnt : RndSys->getActiveEntities())
        {
            auto rTrans = reinterpret_cast<TransformComponent*>(rEnt->getComponent(TransformComponent::Type));
            std::vector<Component*> rndCmpnts = rEnt->getComponents(RndSys->getOptBits());

            for (auto cmpnt : rndCmpnts)
            {
                if (cmpnt == this)
                    continue;

                auto rRnd = reinterpret_cast<RenderComponent*>(cmpnt);

                sf::Vector2f cOffset = grid->mTransform->getInverseTransform().transformPoint(rTrans->getPosition());

                if (cOffset.x >= 0 || cOffset.x+(dim.x/2) <= grid->mSizeX*TILE_SIZE)
                {
                    cOffset.x = fmod(cOffset.x, grid->mSizeX*TILE_SIZE);
                    sf::Transform t = grid->mTransform->getTransform();
                    trans->setPosition(t.transformPoint(cOffset));
                }
            }
        }
    }*/

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
                if (grid->mTiles[y][x].mMat == 0 || grid->mTiles[y][x].mMat >= TileSheets.size())
                    continue;

                // Grab tile sheet info
                sf::Texture* sheet = TileSheets[grid->mTiles[y][x].mMat];
                if (!sheet)
                    continue;

                int sheetSizeX = sheet->getSize().x / TILE_SIZE;
                int sheetSizeY = sheet->getSize().y / TILE_SIZE;

                int edgeState = grid->calcNeighborStateMid(x, y);
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
				for i, c := range grid->mTiles[y][x].comp {
					bheight := (float32(c) / 255 * 16)
					bwidth := float32(TILE_SIZE / MaxComps)
					bstart := start.Add(sf::Vector2f{float32(i) * bwidth, 16 - bheight})

					color := sf::Color{}
					color.A = 255

					switch i + int(grid->mTiles[y][x].mMat) {
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


			for(int j = 0; j < grid->mCTiles.size();j++){
                for (int i = 0; i < grid->getInterestingTiles(j).size(); i++)
                {
                    int u = grid->getInterestingTiles(j)[i].x;
                    int v = grid->getInterestingTiles(j)[i].y;
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

    if (!grid->getWrapX())
    {
        sf::VertexArray physicsPoly(sf::LinesStrip, grid->mPolyShape.getVertices().size()+1);
        for (unsigned int i = 0; i < grid->mPolyShape.getVertices().size(); i++)
        {
            physicsPoly[i].position = grid->mPolyShape.getVertices()[i]*PTU;
            physicsPoly[i].color = sf::Color::White;
        }
        physicsPoly[physicsPoly.getVertexCount()-1] = physicsPoly[0];
        target.draw(physicsPoly, states);
    }

}

void FrontGridComponent::renderShadow(sf::RenderTarget& target, sf::RenderStates states)
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

    for (int _y = top-20; _y <= bot+20; _y++)
	{
		for (int _x = left-20; _x <= right+20; _x++)
        {
            int x = _x;
			int y = _y;

			if (grid->getWrapX())
                x = grid->wrapX(x);

            if (x < 0 || x >= grid->getSizeX() || y < 0 || y >= grid->getSizeY())
                continue;

            grid->mTiles[y][x].mLight = 0;
        }
	}

	for (int _y = top-20; _y <= bot+20; _y++)
	{
		for (int _x = left-20; _x <= right+20; _x++)
        {
            int x = _x;
			int y = _y;

			if (grid->getWrapX())
                x = grid->wrapX(x);

            if (x < 0 || x >= grid->getSizeX() || y < 0 || y >= grid->getSizeY())
                continue;

            if (grid->mTiles[y][x].mBack == 0)
                grid->applyLightRec(x, y, 30);
        }
	}

    sf::VertexArray verts(sf::Quads, 4);
    sf::VertexArray outline(sf::LinesStrip, 5);
	for (int _y = top; _y <= bot; _y++)
	{
		for (int _x = left; _x <= right; _x++)
        {
			int x = grid->wrapX(_x);
			int y = _y;

			if (grid->mTiles[y][x].mLight >= 10)
                continue;

            float intensity = std::min(1.f, float(10-grid->mTiles[y][x].mLight)/10.f);

			auto start = sf::Vector2f(tsize * static_cast<float>(_x), tsize * static_cast<float>(_y)); // Tile start draw
			verts[0] = sf::Vertex(start,
				sf::Color(0, 0, 0, intensity*255),
				sf::Vector2f());
			verts[1] = sf::Vertex(start+sf::Vector2f(0, tsize),
				sf::Color(0, 0, 0, intensity*255),
				sf::Vector2f(0, tsize));
			verts[2] = sf::Vertex(start+sf::Vector2f(tsize, tsize),
				sf::Color(0, 0, 0, intensity*255),
				sf::Vector2f(tsize, tsize));
			verts[3] = sf::Vertex(start+sf::Vector2f(tsize, 0),
				sf::Color(0, 0, 0, intensity*255),
				sf::Vector2f(tsize, 0));

            target.draw(verts, states);
		}
	}
}
