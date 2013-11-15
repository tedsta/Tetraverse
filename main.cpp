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

    RenderSystem *render = new RenderSystem(engine->getEventManager(), ResourceManager::get()->getFont("Content/Fonts/font.ttf"), GridComponent::Type);
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

    // Set up the placeables
    HSQUIRRELVM placeableScript = scriptSys->createScript("Content/Scripts/placeables.nut");
    PlaceableComponent::registerClass(placeableScript, "Door");

    // Set up the materials
    GridComponent::addTileSheet(1, ResourceManager::get()->getTexture("Content/Textures/Tiles/dirt.png"));
    GridComponent::addTileSheet(2, ResourceManager::get()->getTexture("Content/Textures/Tiles/stone.png"));
    GridComponent::addTileSheet(3, ResourceManager::get()->getTexture("Content/Textures/Tiles/grass.png"));
    GridComponent::addTileSheet(5, ResourceManager::get()->getTexture("Content/Textures/Tiles/grass.png"));

    gridSys->addTick(veggyGridOp, 5.f);
    gridSys->addTick(fluidGridOp, 0.05f);
    gridSys->addTick(wireGridOp, 0.01f);

    Scene *scene = engine->getScene();

    Entity *player = new Entity(engine->getEventManager());
    scene->addEntity(player);
    player->addComponent(new TransformComponent);
    player->addComponent(new SpriteComponent("robot.png"));
    player->addComponent(new IntentComponent);
    player->addComponent(new PhysicsComponent);
    player->addComponent(new PlayerComponent);
    InventoryComponent* inventory = new InventoryComponent(10);
    player->addComponent(inventory);

    inventory->addItem(0, 1, 1);
    inventory->addItem(1, 0, 999);
    inventory->addItem(2, 2, 10);

    TransformComponent *trans = static_cast<TransformComponent*>(player->getComponent(TransformComponent::Type));
    IntentComponent *intent = static_cast<IntentComponent*>(player->getComponent(IntentComponent::Type));

    trans->setOrigin(sf::Vector2f(30, 48));
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

    intent->mapMouseBtnToIntent("use", sf::Mouse::Button::Left, BtnState::DOWN);
    intent->mapMouseBtnToIntent("interact", sf::Mouse::Button::Right, BtnState::DOWN);
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
                tiles[y][x].mState = 0;

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
    placeable.Func("setGrid", &PlaceableComponent::setGrid);
    placeable.Func("setClassName", &PlaceableComponent::setClassName);
    Sqrat::RootTable(vm).Bind("PlaceableComponent", placeable);

    Sqrat::Class<Tile> tile(vm);
    tile.Var("mMat", &Tile::mMat);
    Sqrat::RootTable(vm).Bind("Tile", tile);

    Sqrat::DerivedClass<GridComponent, Component> grid(vm);
    grid.Func("canPlace", &GridComponent::canPlace);
    grid.Func("addPlaceable", &GridComponent::addPlaceable);
    grid.Func("getPlaceableAt", &GridComponent::getPlaceableAt);
    grid.Func("setTile", &GridComponent::setTile);
    grid.Func("getTile", &GridComponent::getTile);
    Sqrat::RootTable(vm).Bind("GridComponent", grid);
}
