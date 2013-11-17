

#include <iostream>
#include <bitset>
#include <cstdlib>
#include <sstream>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <Sqrat/sqext.h>

#include <Fission/Core/Engine.h>
#include <Fission/Core/Scene.h>
#include <Fission/Core/ResourceManager.h>
#include <Fission/Core/ComponentFactories.h>

#include <Fission/Rendering/RenderSystem.h>
#include <Fission/Rendering/DebugDisplay.h>
#include <Fission/Rendering/SpriteComponent.h>
#include <Fission/Rendering/TransformComponent.h>

#include <Fission/Input/InputSystem.h>

#include <Fission/Network/Connection.h>
#include <Fission/Network/IntentSystem.h>
#include <Fission/Network/IntentComponent.h>

#include <Fission/Script/ScriptSystem.h>
#include <Fission/Script/ScriptComponent.h>


#include "GridComponent.h"
#include "PhysicsComponent.h"
#include "SkeletonComponent.h"
#include "PlayerComponent.h"
#include "PlaceableComponent.h"
#include "InventoryComponent.h"
#include "ItemComponent.h"

#include "GridSystem.h"
#include "PlaceableSystem.h"
#include "PhysicsSystem.h"
#include "PlayerSystem.h"

#include "GridOps.h"

#include "perlin/perlin.h"

Tile** newWorld(int seed, int width, int height);
void bindSquirrel(HSQUIRRELVM vm);

int main()
{
    ResourceManager::init();

    Engine *engine = new Engine;

    TransformComponent::Type = ComponentFactories::add(TransformComponent::factory);
    SpriteComponent::Type = ComponentFactories::add(SpriteComponent::factory);
    ScriptComponent::Type = ComponentFactories::add(ScriptComponent::factory);
    IntentComponent::Type = ComponentFactories::add(IntentComponent::factory);
    GridComponent::Type = ComponentFactories::add(GridComponent::factory);
    PhysicsComponent::Type = ComponentFactories::add(PhysicsComponent::factory);
    SkeletonComponent::Type = ComponentFactories::add(SkeletonComponent::factory);
    PlayerComponent::Type = ComponentFactories::add(PlayerComponent::factory);
    PlaceableComponent::Type = ComponentFactories::add(PlaceableComponent::factory);
    InventoryComponent::Type = ComponentFactories::add(InventoryComponent::factory);
    ItemComponent::Type = ComponentFactories::add(ItemComponent::factory);

    Connection* conn = new Connection(engine->getEventManager());

    RenderSystem *render = new RenderSystem(engine->getEventManager(), ResourceManager::get()->getFont("Content/Fonts/font.ttf"),
                                            GridComponent::Type|SkeletonComponent::Type);
    InputSystem *input = new InputSystem(engine->getEventManager(), &render->getWindow());
    IntentSystem *intentSys = new IntentSystem(engine->getEventManager(), conn);
    ScriptSystem *scriptSys = new ScriptSystem(engine->getEventManager(), engine);
    GridSystem *gridSys = new GridSystem(engine->getEventManager());
    PlaceableSystem *placeableSys = new PlaceableSystem(engine->getEventManager());
    PhysicsSystem *physSys = new PhysicsSystem(engine->getEventManager());
    PlayerSystem *playerSys = new PlayerSystem(engine->getEventManager(), render);

    engine->addSystem(render);
    engine->addSystem(input);
    engine->addSystem(intentSys);
    engine->addSystem(scriptSys);
    engine->addSystem(gridSys);
    engine->addSystem(placeableSys);
    engine->addSystem(physSys);
    engine->addSystem(playerSys);

    render->setBackgroundColor(sf::Color(130, 130, 255, 255));

    scriptSys->addBinder(bindSquirrel);

    // Set up all the items
    HSQUIRRELVM itemScript = scriptSys->createScript("Content/Scripts/items.nut");
    Item *dirtBlock = new Item("dirt", "Content/Textures/Tiles/dirt.png", true, 999, itemScript, "dirtBlock");
    Item::Items.push_back(dirtBlock);

    Item *crowbar = new Item("crowbar", "Content/Textures/Tiles/dirt.png", false, 1, itemScript, "crowbar");
    Item::Items.push_back(crowbar);

    Item *door = new Item("door", "Content/Textures/Tiles/dirt.png", true, 10, itemScript, "door");
    Item::Items.push_back(door);

    Item *water = new Item("water", "Content/Textures/Tiles/dirt.png", false, 1, itemScript, "water");
    Item::Items.push_back(water);

    // Set up the placeables
    HSQUIRRELVM placeableScript = scriptSys->createScript("Content/Scripts/placeables.nut");
    PlaceableComponent::registerClass(placeableScript, "Door");

    // Set up the materials
    GridComponent::addTileSheet(1, ResourceManager::get()->getTexture("Content/Textures/Tiles/dirt.png"));
    GridComponent::addTileSheet(2, ResourceManager::get()->getTexture("Content/Textures/Tiles/stone.png"));
    GridComponent::addTileSheet(3, ResourceManager::get()->getTexture("Content/Textures/Tiles/grass.png"));
    GridComponent::addTileSheet(5, ResourceManager::get()->getTexture("Content/Textures/Tiles/grass.png"));

    gridSys->addTick(veggyGridOp, 5.f);
    gridSys->addTick(fluidGridOp2, 0.005f);
    gridSys->addTick(wireGridOp, 0.01f);

    Scene *scene = engine->getScene();

    Entity *player = new Entity(engine->getEventManager());
    scene->addEntity(player);
    player->addComponent(new TransformComponent);
    //player->addComponent(new SpriteComponent("robot.png"));
    player->addComponent(new SkeletonComponent("Content/Spine/player.json", "Content/Spine/player.atlas"));
    player->addComponent(new IntentComponent);
    player->addComponent(new PhysicsComponent);
    player->addComponent(new PlayerComponent);
    InventoryComponent* inventory = new InventoryComponent(10);
    player->addComponent(inventory);

    inventory->addItem(0, 1, 1);
    inventory->addItem(1, 0, 999);
    inventory->addItem(2, 2, 10);
    inventory->addItem(3, 3, 1);

    TransformComponent *trans = static_cast<TransformComponent*>(player->getComponent(TransformComponent::Type));
    IntentComponent *intent = static_cast<IntentComponent*>(player->getComponent(IntentComponent::Type));

    //trans->setOrigin(sf::Vector2f(30, 48));
    trans->setPosition(sf::Vector2f(3.f, 3.f));
    intent->mapKeyToIntent("up", sf::Keyboard::W, BtnState::DOWN);
    intent->mapKeyToIntent("down", sf::Keyboard::S, BtnState::DOWN);
    intent->mapKeyToIntent("left", sf::Keyboard::A, BtnState::DOWN);
    intent->mapKeyToIntent("right", sf::Keyboard::D, BtnState::DOWN);

    intent->mapKeyToIntent("0", sf::Keyboard::Num0, BtnState::DOWN);
    intent->mapKeyToIntent("1", sf::Keyboard::Num1, BtnState::DOWN);
    intent->mapKeyToIntent("2", sf::Keyboard::Num2, BtnState::DOWN);
    intent->mapKeyToIntent("3", sf::Keyboard::Num3, BtnState::DOWN);
    intent->mapKeyToIntent("4", sf::Keyboard::Num4, BtnState::DOWN);
    intent->mapKeyToIntent("5", sf::Keyboard::Num5, BtnState::DOWN);
    intent->mapKeyToIntent("6", sf::Keyboard::Num6, BtnState::DOWN);
    intent->mapKeyToIntent("7", sf::Keyboard::Num7, BtnState::DOWN);
    intent->mapKeyToIntent("8", sf::Keyboard::Num8, BtnState::DOWN);
    intent->mapKeyToIntent("9", sf::Keyboard::Num9, BtnState::DOWN);

    intent->mapMouseBtnToIntent("useLeft", sf::Mouse::Button::Left, BtnState::DOWN);
    intent->mapMouseBtnToIntent("useRight", sf::Mouse::Button::Right, BtnState::DOWN);
    intent->mapKeyToIntent("interact", sf::Keyboard::E, BtnState::PRESSED);
    intent->mapKeyToIntent("test", sf::Keyboard::T, BtnState::PRESSED);

    int worldW = 1000;
    int worldH = 1000;
    Tile** tiles = newWorld(0, worldW, worldH);

    for (int i = 0; i < 1; i++)
    {
        TransformComponent* pt = new TransformComponent(sf::Vector2f(0, 0));

        Entity *planet = new Entity(engine->getEventManager());
        scene->addEntity(planet);
        planet->addComponent(pt);
        planet->addComponent(new GridComponent(pt, worldW, worldH, tiles, 3));
        planet->addComponent(new ScriptComponent(scriptSys->createScript("test.nut")));
        physSys->addGrid(planet);
    }

    float accum = 0;
    int frames = 0;

    sf::Clock clock;
    while (render->getWindow().isOpen())
    {
        //while (clock.getElapsedTime().asMicroseconds() < 1000);

        float dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        accum += dt;

        if (accum >= 1.f)
        {
            std::stringstream fps;
            std::string sFps;
            fps << frames;
            fps >> sFps;
            render->getDebugDisplay()->setMessage("FPS", sFps);
            //std::cout << "FPS: " << frames << std::endl;
            accum = 0;
            frames = 0;
        }

        frames++;

        engine->update(dt);
    }

    delete engine;

    return 0;
}

Tile** newWorld(int seed, int width, int height)
{
	Tile** tiles = new Tile*[height];
	for (int i = 0; i < height; i++)
		tiles[i] = new Tile[width];

	for (int y = 0; y < height; y++)
    {
		for (int x = 0; x < width; x++)
		{
			auto n = PerlinNoise1D(x, 1.01, .02, 2) + 1;
			auto p = PerlinNoise2D(y, x, 1.01, 0.2, 10) + 1;
			p += float(height-y) / float(height);
			float o[MAX_COMPS];
			for (int i = 0; i < MAX_COMPS; i++)
			{
				o[i] = PerlinNoise2D(x, y, 1.01, 0.2, i) + 1;
			}
			if (y > n*100)
			{
                tiles[y][x].mMat = p * 3 / 2;

				if (tiles[y][x].mMat > 3)
					tiles[y][x].mMat = 3;

				for (int i = 0; i < MAX_COMPS; i++)
                {
					tiles[y][x].mComp[i] = o[i] * 128;
				}
				//tiles[y][x].mHeat = p * 256;
			}
		}
	}
	return tiles;
}

void bindSquirrel(HSQUIRRELVM vm)
{
    Sqrat::DerivedClass<TransformComponent, Component, sqext::ConstAlloc<TransformComponent, sf::Vector2f, float, sf::Vector2f>> transform(vm);
    transform.Func("setPosition", (void (TransformComponent::*)(const sf::Vector2f&))&TransformComponent::setPosition);
    transform.Func("getPosition", &TransformComponent::getPosition);
    Sqrat::RootTable(vm).Bind("TransformComponent", transform);

    Sqrat::DerivedClass<SpriteComponent, Component, sqext::ConstAlloc<SpriteComponent, const std::string&, int, int>> sprite(vm);
    //sprite.Func("setTexture", &SpriteComponent::setTexture);
    Sqrat::RootTable(vm).Bind("SpriteComponent", sprite);

    Sqrat::DerivedClass<PlaceableComponent, Component, sqext::ConstAlloc<PlaceableComponent, GridComponent*, const std::string&, int, int, int, int>> placeable(vm);
    Sqrat::RootTable(vm).Bind("PlaceableComponent", placeable);

    Sqrat::Class<Tile> tile(vm);
    tile.Var("mMat", &Tile::mMat);
    tile.Var("mWire", &Tile::mWire);
    tile.Var("mFluid", &Tile::mFluid);
    tile.Var("mSignal", &Tile::mSignal);
    Sqrat::RootTable(vm).Bind("Tile", tile);

    Sqrat::DerivedClass<GridComponent, Component> grid(vm);
    grid.Func("canPlace", &GridComponent::canPlace);
    grid.Func("addPlaceable", &GridComponent::addPlaceable);
    grid.Func("getPlaceableAt", &GridComponent::getPlaceableAt);
    grid.Func("setTile", &GridComponent::setTile);
    grid.Func("getTile", &GridComponent::getTile);
    Sqrat::RootTable(vm).Bind("GridComponent", grid);
}

/*

#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <SFML/Window/Window.hpp>

using namespace std;

// simple Eigen::Matrix work-alike
template< typename T >
class Matrix
{
public:
    Matrix( const size_t rows, const size_t cols )
        : mStride( cols )
        , mHeight( rows )
        , mStorage( rows * cols )
    {}

    T& operator()( const size_t row, const size_t col )
    {
        return mStorage[ row * mStride + col ];
    }

    const T& operator()( const size_t row, const size_t col ) const
    {
        return mStorage[ row * mStride + col ];
    }

    size_t rows() const { return mHeight; }
    size_t cols() const { return mStride; }

private:
    vector< T > mStorage;
    size_t mStride;
    size_t mHeight;
};

struct Cell
{
    enum Type{ AIR, GROUND, WATER };

    Cell()
        : mType( AIR )
        , mMass( 0 )
        , mNewMass( 0 )
    {}

    Type mType;
    float mMass;
    float mNewMass;
};

const float MaxMass = 1.f;
const float MinMass = 0.01;
const float MaxCompress = 0.02;
const float MaxSpeed = 0.1f;
const float MinFlow = 0.1;

//Take an amount of water and calculate how it should be split among two
//vertically adjacent cells. Returns the amount of water that should be in
//the bottom cell.
float get_stable_state_b( float total_mass )
{
    if ( total_mass <= MaxMass )
    {
        return MaxMass;
    }
    else if ( total_mass < 2*MaxMass + MaxCompress )
    {
        return ((MaxMass + total_mass*MaxCompress)/(MaxMass + MaxMass*MaxCompress))*MaxMass;
    }
    else
    {
        return (total_mass + MaxCompress)/2;
    }
}

template< typename T >
T constrain( const T& val, const T& minVal, const T& maxVal )
{
    return max( minVal, min( val, maxVal ) );
}

typedef Matrix< Cell > State;
void stepState( State& cur )
{
    for( size_t y = 1; y < cur.rows()-1; ++y )
    {
        for( size_t x = 1; x < cur.cols()-1; ++x )
        {
            Cell& center = cur( y, x );

            // Skip inert ground blocks
            if( center.mType == Cell::GROUND )
                continue;

            // Custom push-only flow
            float Flow = 0;
            float remaining_mass = center.mMass;
            if( remaining_mass <= 0 )
                continue;

            // The block below this one
            Cell& below = cur( y-1, x );
            if( below.mType != Cell::GROUND )
            {
                Flow = get_stable_state_b( remaining_mass + below.mMass ) - below.mMass;
                if( Flow > MinFlow )
                {
                    //leads to smoother flow
                    //Flow /= 2.f;
                }
                Flow = constrain( Flow, 0.f, min(MaxSpeed, remaining_mass) );

                center.mNewMass -= Flow;
                below.mNewMass += Flow;
                remaining_mass -= Flow;
            }

            if ( remaining_mass <= 0 )
                continue;

            // Left
            Cell& left = cur( y, x-1 );
            if ( left.mType != Cell::GROUND )
            {
                // Equalize the amount of water in this block and it's neighbour
                Flow = ( center.mMass - left.mMass ) / 4;
                if ( Flow > MinFlow )
                {
                    //Flow /= 2.f;
                }
                Flow = constrain(Flow, 0.f, remaining_mass);
                center.mNewMass -= Flow;
                left.mNewMass += Flow;
                remaining_mass -= Flow;
            }

            if ( remaining_mass <= 0 )
                continue;

            // Right
            Cell& right = cur( y, x+1 );
            if ( right.mType != Cell::GROUND )
            {
                // Equalize the amount of water in this block and it's neighbour
                Flow = ( center.mMass - right.mMass ) / 4;
                if ( Flow > MinFlow )
                {
                    //Flow /= 2.f;
                }
                Flow = constrain(Flow, 0.f, remaining_mass);
                center.mNewMass -= Flow;
                right.mNewMass += Flow;
                remaining_mass -= Flow;
            }

            if ( remaining_mass <= 0 )
                continue;

            // The block above this one
            Cell& above = cur( y+1, x );
            if( above.mType != Cell::GROUND )
            {
                Flow = remaining_mass - get_stable_state_b( remaining_mass + above.mMass );
                if( Flow > MinFlow )
                {
                    //leads to smoother flow
                    //Flow /= 2.f;
                }
                Flow = constrain( Flow, 0.f, min(MaxSpeed, remaining_mass) );

                center.mNewMass -= Flow;
                above.mNewMass += Flow;
                remaining_mass -= Flow;
            }
        }
    }

    for( size_t y = 0; y < cur.rows(); ++y )
    {
        for( size_t x = 0; x < cur.cols(); ++x )
        {
            cur( y, x ).mMass = cur( y, x ).mNewMass;
        }
    }

    for( size_t y = 0; y < cur.rows(); ++y )
    {
        for( size_t x = 0; x < cur.cols(); ++x )
        {
            Cell& center = cur( y, x );
            if( center.mType == Cell::GROUND )
            {
                center.mMass = center.mNewMass = 0.0f;
                continue;
            }
            if( center.mMass > MinMass )
            {
                center.mType = Cell::WATER;
            }
            else
            {
                center.mType = Cell::AIR;
                center.mMass = 0.0f;
            }
        }
    }

    // Remove any water that has left the map
    for( size_t x = 0; x < cur.cols(); ++x )
    {
        cur( 0, x ).mMass = 0;
        cur( cur.rows()-1, x ).mMass = 0;
    }
    for( size_t y = 0; y < cur.rows(); ++y )
    {
        cur( y, 0 ).mMass = 0;
        cur( y, cur.cols()-1 ).mMass = 0;
    }
}

void showState( const State& state )
{
    glPolygonMode( GL_FRONT, GL_LINE );
    glBegin( GL_QUADS );
    glColor3ub( 0, 0, 0 );
    for( size_t y = 0; y < state.rows(); ++y )
    {
        for( size_t x = 0; x < state.cols(); ++x )
        {
            glVertex2f( x+0, y+0 );
            glVertex2f( x+1, y+0 );
            glVertex2f( x+1, y+1 );
            glVertex2f( x+0, y+1 );
        }
    }
    glEnd();

    glPolygonMode( GL_FRONT, GL_FILL );
    glBegin( GL_QUADS );
    for( size_t y = 0; y < state.rows(); ++y )
    {
        for( size_t x = 0; x < state.cols(); ++x )
        {
            if( state( y, x ).mType == Cell::AIR )
                continue;

            float height = 1.0f;
            if( state( y, x ).mType == Cell::GROUND )
            {
                glColor3ub( 152, 118, 84 );
            }
            else
            {
                glColor3ub( 0, 135, 189 );
                height = min( 1.f, (float)state( y, x ).mMass/(float)MaxMass );
            }

            glVertex2f( x+0, y );
            glVertex2f( x+1, y );
            glVertex2f( x+1, y + height );
            glVertex2f( x+0, y + height );
        }
    }
    glEnd();
}

State state( 20, 20 );
void mouse( int button, int button_state, int x, int y )
{
    float pctX = (float)x / glutGet( GLUT_WINDOW_WIDTH );
    float pctY = 1.0f - ( (float)y / glutGet( GLUT_WINDOW_HEIGHT ) );
    size_t cellX = pctX * state.cols();
    size_t cellY = pctY * state.rows();
    Cell& cur = state( cellY, cellX );

    if( button_state == GLUT_UP )
        return;

    if( button == GLUT_LEFT_BUTTON )
    {
        cur.mType = ( cur.mType == Cell::GROUND ? Cell::AIR : Cell::GROUND );
        cur.mMass = cur.mNewMass = 0.0f;
    }

    if( button == GLUT_RIGHT_BUTTON )
    {
        cur.mType = Cell::WATER;
        cur.mMass = cur.mNewMass = 20.f;
    }
}


void display()
{
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;
        for( size_t y = 0; y < state.rows(); ++y )
        {
            for( size_t x = 0; x < state.cols(); ++x )
            {
                state( y, x ).mType = (Cell::Type)( rand() % 3 );
                state( y, x ).mMass = 1.0f;
                state( y, x ).mNewMass = 1.0f;
            }
        }
    }

    glClearColor( 1, 1, 1, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, state.cols(), 0, state.rows(), -1, 1);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    stepState( state );
    showState( state );

    glutSwapBuffers();

    glutPostRedisplay();
}

void timer(int extra)
{
    glutTimerFunc(16, timer, 0);
}

void idle()
{
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( 640, 480 );
    glutCreateWindow( "Cells" );
    glutDisplayFunc( display );
    glutIdleFunc(idle);
    glutMouseFunc( mouse );
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    return 0;
}*/
