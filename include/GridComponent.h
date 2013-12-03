#ifndef GRIDCOMPONENT_H
#define GRIDCOMPONENT_H

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <Fission/Rendering/RenderComponent.h>

#include "phys/Manifold.h"
#include "phys/RigidBody.h"

#define MAX_COMPS 4
#define TILE_SIZE 16

enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NO_DIR
};

struct Tile
{
    Tile() : mMat(0), mFluid(0), mWire(0), mSignal(0) {}
    Tile(sf::Uint8 mat, sf::Uint8 fluid = 0, sf::Uint8 wire = 0, sf::Uint8 signal = 0) :
        mMat(mat), mFluid(fluid), mWire(wire), mSignal(signal) {}

    bool isInteresting(){return mFlags&1;}
    void setInteresting(){mFlags|=1;}
    void clearInteresting(){mFlags&=~1;}

    bool isSolid(){return mFlags&2;}
    void setSolid(){mFlags|=2;}
    void clearSolid(){mFlags&=~2;}

	sf::Uint8 mMat;
	sf::Uint8 mFlags;

	// status
	sf::Uint8 mComp[MAX_COMPS]; // composit id, quantity
	sf::Uint8 mVeggy;
	float mFluid;
	sf::Uint8 mWire;
	sf::Uint8 mSignal;
};

struct Area
{
    Area() : mChanged(false) {}

	Tile mTiles[3][3];
	int mX, mY;
	bool mChanged;
};

class Entity;

class GridComponent : public RenderComponent
{
    friend class GridSystem;
    friend class GridShape;

    friend void gridToPolygon(phys::Manifold* m, phys::RigidBody* a, phys::RigidBody* b);

    public:
        GridComponent(sf::Transformable* transform = NULL, int sizeX = 0, int sizeY = 0, bool wrapX = false, Tile** tiles = NULL, int tickCount = 0);
        virtual ~GridComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

        // Renderable components gotto render...
        void render(sf::RenderTarget& target, sf::RenderStates states);
        void renderShadow(sf::RenderTarget& target, sf::RenderStates states);

        sf::Vector2f getTilePos(sf::Vector2f pos);

        void sliceInto(Entity* newGrid, int left, int top, int right, int bot);

        void interact(int x, int y);
        void addFluid(int x, int y, float fluid);

        void setTile(int x, int y, Tile tile, int tick);
        bool canPlace(int x, int y, int width, int height);
        void addPlaceable(Entity* entity);
        void removePlaceable(Entity* placeable);
        Entity* getPlaceableAt(int x, int y);
        int calcNeighborState(int x, int y);
        int wrapX(int x) const;

        void setInteresting(int x, int y, int tick)
        {
            if (std::find(mCTiles[tick].begin(), mCTiles[tick].end(), sf::Vector2i(x, y)) == mCTiles[tick].end())
                mCTiles[tick].push_back(sf::Vector2i(x, y));
        }
        const std::vector<sf::Vector2i>& getInterestingTiles(int tick) const {return mCTiles[tick];}
        void clearInteresting(int tick){mCTiles[tick].clear();}

        void addChild(Entity* child){mChildren.push_back(child);}

        // Getters
        bool getWrapX() const {return mWrapX;}
        Tile getTile(int x, int y) const;
        Area getArea(int x, int y) const;
        int getSizeX() const {return mSizeX;}
        int getSizeY() const {return mSizeY;}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new GridComponent;}

        static void addTileSheet(int mat, sf::Texture* sheet)
        {
            if (mat >= static_cast<int>(TileSheets.size()))
                TileSheets.resize(mat+1);
            TileSheets[mat] = sheet;
        }

    private:
        int mID;
        int mSizeX;
        int mSizeY;
        bool mWrapX;
        Tile** mTiles; // 2D array of tiles
        sf::Transformable* mTransform;
        int mTickCount; // The number of tick types
        std::vector<std::vector<sf::Vector2i>> mCTiles; // Cached interesting tile coordinates
        std::vector<Entity*> mPlaceables;
        std::vector<Entity*> mChildren; // All of the entities in this grid

        static std::vector<sf::Texture*> TileSheets;
};

#endif // GRIDCOMPONENT_H
