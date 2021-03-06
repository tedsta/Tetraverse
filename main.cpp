

#include <iostream>
#include <bitset>
#include <cstdlib>
#include <sstream>
#include <stack>

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

#include "phys/PhysicsWorld.h"
#include "phys/RigidBody.h"
#include "phys/Shape.h"

#include "GridComponent.h"
#include "FrontGridComponent.h"
#include "BackGridComponent.h"
#include "PhysicsComponent.h"
#include "SkeletonComponent.h"
#include "PlayerComponent.h"
#include "PlaceableComponent.h"
#include "InventoryComponent.h"
#include "ItemComponent.h"
#include "LightComponent.h"
#include "SignalComponent.h"
#include "WeaponComponent.h"
#include "WrapComponent.h"
#include "EmitterComponent.h"

#include "GridSystem.h"
#include "PlaceableSystem.h"
#include "PhysicsSystem.h"
#include "PlayerSystem.h"
#include "LightSystem.h"
#include "EmitterSystem.h"

#include "GridOps.h"

#include "TVNetwork.h"
#include "PlayerDatabase.h"

#include "perlin/perlin.h"
#include <random>

void runServer();
void runClient();
Tile** newWorld(int seed, int width, int height);
void bindSquirrel(HSQUIRRELVM vm);

int main()
{
    ResourceManager::init();

    TransformComponent::Type = ComponentFactories::add(TransformComponent::factory);
    SpriteComponent::Type = ComponentFactories::add(SpriteComponent::factory);
    ScriptComponent::Type = ComponentFactories::add(ScriptComponent::factory);
    IntentComponent::Type = ComponentFactories::add(IntentComponent::factory);
    GridComponent::Type = ComponentFactories::add(GridComponent::factory);
    BackGridComponent::Type = ComponentFactories::add(BackGridComponent::factory);
    FrontGridComponent::Type = ComponentFactories::add(FrontGridComponent::factory);
    PhysicsComponent::Type = ComponentFactories::add(PhysicsComponent::factory);
    SkeletonComponent::Type = ComponentFactories::add(SkeletonComponent::factory);
    PlayerComponent::Type = ComponentFactories::add(PlayerComponent::factory);
    PlaceableComponent::Type = ComponentFactories::add(PlaceableComponent::factory);
    InventoryComponent::Type = ComponentFactories::add(InventoryComponent::factory);
    ItemComponent::Type = ComponentFactories::add(ItemComponent::factory);
    LightComponent::Type = ComponentFactories::add(LightComponent::factory);
    SignalComponent::Type = ComponentFactories::add(SignalComponent::factory);
    WeaponComponent::Type = ComponentFactories::add(WeaponComponent::factory);
    WrapComponent::Type = ComponentFactories::add(WrapComponent::factory);
    EmitterComponent::Type = ComponentFactories::add(EmitterComponent::factory);

    std::cout << "Tetraverse\nTeam Fission\n2013\n\nMake a selection:\n";
    std::cout << "1. Server\n";
    std::cout << "2. Client\n";

    bool valid = false;

    while (!valid)
    {
        int sel;
        std::cin >> sel;

        switch (sel)
        {
        case 1:
            valid = true;
            runServer();
            break;
        case 2:
            valid = true;
            runClient();
            break;
        default:
            std::cout << "Invalid selection.\n";
            break;
        }
    }


    return 0;
}

void runServer()
{
    Engine *engine = new Engine;

    Connection* conn = new Connection(engine->getEventManager());
    //conn->hostServer(9999);

    PlayerDatabase* playerDB = new PlayerDatabase(engine);
    TVNetwork* network = new TVNetwork(engine, conn, playerDB);

    RenderSystem *render = new RenderSystem(engine->getEventManager(), 0.f, ResourceManager::get()->getFont("Content/Fonts/font.ttf"),
                                            BackGridComponent::Type|FrontGridComponent::Type|SkeletonComponent::Type|WrapComponent::Type);
    InputSystem *input = new InputSystem(engine->getEventManager(), 1.f/60.f, &render->getWindow());
    IntentSystem *intentSys = new IntentSystem(engine->getEventManager(), 1.f/60.f, conn);
    ScriptSystem *scriptSys = new ScriptSystem(engine->getEventManager(), 1.f/60.f, engine);
    PhysicsSystem *physSys = new PhysicsSystem(engine->getEventManager(), 1.f/60.f);
    GridSystem *gridSys = new GridSystem(engine->getEventManager(), physSys, 1.f/1000.f);
    PlaceableSystem *placeableSys = new PlaceableSystem(engine->getEventManager(), 1.f/60.f);
    PlayerSystem *playerSys = new PlayerSystem(engine->getEventManager(), render, conn, 1.f/60.f);
    LightSystem* lightSys = new LightSystem(engine->getEventManager(), render, 0.f);

    engine->addSystem(render);
    engine->addSystem(input);
    engine->addSystem(intentSys);
    engine->addSystem(scriptSys);
    engine->addSystem(playerSys);
    engine->addSystem(placeableSys);
    engine->addSystem(physSys);
    engine->addSystem(gridSys);
    engine->addSystem(lightSys);

    FrontGridComponent::RndSys = render;

    render->setBackgroundColor(sf::Color(130, 130, 255, 255));

    scriptSys->addBinder(bindSquirrel);

    // Set up all the items
    HSQUIRRELVM itemScript = scriptSys->createScript("Content/Scripts/items.nut");
    Item *dirtBlock = new Item("dirt block", "Content/Textures/Tiles/dirt.png", true, BtnState::DOWN, 999, 1, itemScript, "dirtBlock");
    Item::Items.push_back(dirtBlock);

    Item *steelBlock = new Item("steel block", "Content/Textures/Tiles/dirt.png", true, BtnState::DOWN, 999, 1, itemScript, "steelBlock");
    Item::Items.push_back(steelBlock);

    Item *steelWall = new Item("steel wall", "Content/Textures/Tiles/dirt.png", true, BtnState::DOWN, 999, 1, itemScript, "steelWall");
    Item::Items.push_back(steelWall);

    Item *crowbar = new Item("crowbar", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 1, 1, itemScript, "crowbar");
    Item::Items.push_back(crowbar);

    Item *door = new Item("door", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 10, 1, itemScript, "door");
    Item::Items.push_back(door);

    Item *water = new Item("water", "Content/Textures/Tiles/dirt.png", false, BtnState::DOWN, 1, 1, itemScript, "water");
    Item::Items.push_back(water);

    Item *thruster = new Item("thruster", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 10, 1, itemScript, "thruster");
    Item::Items.push_back(thruster);

    Item *gridCutter = new Item("gridCutter", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 1, 2, itemScript, "gridCutter");
    Item::Items.push_back(gridCutter);

    Item *wirer = new Item("wirer", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 1, 2, itemScript, "wirer");
    Item::Items.push_back(wirer);

    Item *light = new Item("light", "Content/Textures/Placeables/light.png", false, BtnState::PRESSED, 1, 1, itemScript, "light");
    Item::Items.push_back(light);

    Item *swtch = new Item("switch", "Content/Textures/Placeables/switch.png", false, BtnState::PRESSED, 1, 1, itemScript, "swtch");
    Item::Items.push_back(swtch);

    Item *o2 = new Item("o2", "Content/Textures/Placeables/o2.png", false, BtnState::PRESSED, 1, 1, itemScript, "o2");
    Item::Items.push_back(o2);

    Item *reactor = new Item("reactor", "Content/Textures/Placeables/reactor.png", false, BtnState::PRESSED, 1, 1, itemScript, "reactor");
    Item::Items.push_back(reactor);

    // Set up the placeables
    HSQUIRRELVM placeableScript = scriptSys->createScript("Content/Scripts/placeables.nut");
    PlaceableComponent::registerClass(placeableScript, "Door");
    PlaceableComponent::registerClass(placeableScript, "Thruster");
    PlaceableComponent::registerClass(placeableScript, "Light");
    PlaceableComponent::registerClass(placeableScript, "Swtch");

    // Set up the materials
    FrontGridComponent::addTileSheet(1, ResourceManager::get()->getTexture("Content/Textures/Tiles/dirt.png"));
    FrontGridComponent::addTileSheet(2, ResourceManager::get()->getTexture("Content/Textures/Tiles/stone.png"));
    FrontGridComponent::addTileSheet(3, ResourceManager::get()->getTexture("Content/Textures/Tiles/grass.png"));
    FrontGridComponent::addTileSheet(5, ResourceManager::get()->getTexture("Content/Textures/Tiles/steel.png"));
    FrontGridComponent::addTileSheet(6, ResourceManager::get()->getTexture("Content/Textures/Tiles/steel_window.png"));
    FrontGridComponent::addTileSheet(7, ResourceManager::get()->getTexture("Content/Textures/Tiles/glass.png"));

    BackGridComponent::addTileSheet(1, ResourceManager::get()->getTexture("Content/Textures/Tiles/dirt.png"));
    BackGridComponent::addTileSheet(2, ResourceManager::get()->getTexture("Content/Textures/Tiles/stone.png"));
    BackGridComponent::addTileSheet(3, ResourceManager::get()->getTexture("Content/Textures/Tiles/grass.png"));
    BackGridComponent::addTileSheet(5, ResourceManager::get()->getTexture("Content/Textures/Tiles/steel_wall.png"));
    BackGridComponent::addTileSheet(6, ResourceManager::get()->getTexture("Content/Textures/Tiles/steel_window.png"));
    BackGridComponent::addTileSheet(7, ResourceManager::get()->getTexture("Content/Textures/Tiles/glass.png"));

    gridSys->addTick(veggyGridOp, 5.f);
    gridSys->addTick(fluidGridOp, 0.001f);
    //gridSys->addTick(lightGridOp, 0.1f);

    Scene *scene = engine->getScene();

    int worldW = 50;
    int worldH = 150;

    Tile** tiles = newWorld(0, worldW, worldH);

    Entity *planet = new Entity(engine->getEventManager());
    planet->giveID();
    scene->addEntity(planet);
    TransformComponent* pt = new TransformComponent(sf::Vector2f(0, 0));
    planet->addComponent(pt);
    GridComponent* pg = new GridComponent(pt, worldW, worldH, true, tiles, 2);
    planet->addComponent(pg);
    planet->addComponent(new BackGridComponent(pg));
    planet->addComponent(new FrontGridComponent(pg));
    planet->addComponent(new PhysicsComponent(pg));

    playerDB->createPlayer("asdf", "asdf");
    playerDB->loginPlayer("asdf", 0);

    float accum = 0;
    int frames = 0;

    sf::Clock saveClock;

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
            accum = 0;
            frames = 0;
        }
        frames++;

        conn->update(dt);
        engine->update(dt);

        if (saveClock.getElapsedTime().asSeconds() >= 10.f)
        {
            saveClock.restart();
            //engine->getScene()->save("myfunscene.tsc");
        }
    }

    delete engine;
}

void runClient()
{
    std::string user, pass;
    std::cout << "User: ";
    std::cin >> user;
    std::cout << "Password: ";
    std::cin >> pass;

    Engine *engine = new Engine;

    Connection* conn = new Connection(engine->getEventManager());
    conn->connectClient("localhost", 9999);

    PlayerDatabase* playerDB = new PlayerDatabase(engine);
    TVNetwork* network = new TVNetwork(engine, conn, playerDB);

    RenderSystem *render = new RenderSystem(engine->getEventManager(), 0.f, ResourceManager::get()->getFont("Content/Fonts/font.ttf"),
                                            BackGridComponent::Type|FrontGridComponent::Type|SkeletonComponent::Type|WrapComponent::Type);
    InputSystem *input = new InputSystem(engine->getEventManager(), 1.f/60.f, &render->getWindow());
    IntentSystem *intentSys = new IntentSystem(engine->getEventManager(), 1.f/60.f, conn);
    ScriptSystem *scriptSys = new ScriptSystem(engine->getEventManager(), 1.f/60.f, engine);
    PhysicsSystem *physSys = new PhysicsSystem(engine->getEventManager(), 1.f/60.f);
    GridSystem *gridSys = new GridSystem(engine->getEventManager(), physSys, 1.f/1000.f);
    PlaceableSystem *placeableSys = new PlaceableSystem(engine->getEventManager(), 1.f/60.f);
    PlayerSystem *playerSys = new PlayerSystem(engine->getEventManager(), render, conn, 1.f/60.f);
    LightSystem* lightSys = new LightSystem(engine->getEventManager(), render, 0.f);

    engine->addSystem(render);
    engine->addSystem(input);
    engine->addSystem(intentSys);
    engine->addSystem(scriptSys);
    engine->addSystem(playerSys);
    engine->addSystem(placeableSys);
    engine->addSystem(physSys);
    engine->addSystem(gridSys);
    engine->addSystem(lightSys);

    FrontGridComponent::RndSys = render;

    render->setBackgroundColor(sf::Color(130, 130, 255, 255));

    scriptSys->addBinder(bindSquirrel);

    // Set up all the items
    HSQUIRRELVM itemScript = scriptSys->createScript("Content/Scripts/items.nut");
    Item *dirtBlock = new Item("dirt block", "Content/Textures/Tiles/dirt.png", true, BtnState::DOWN, 999, 1, itemScript, "dirtBlock");
    Item::Items.push_back(dirtBlock);

    Item *steelBlock = new Item("steel block", "Content/Textures/Tiles/dirt.png", true, BtnState::DOWN, 999, 1, itemScript, "steelBlock");
    Item::Items.push_back(steelBlock);

    Item *steelWall = new Item("steel wall", "Content/Textures/Tiles/dirt.png", true, BtnState::DOWN, 999, 1, itemScript, "steelWall");
    Item::Items.push_back(steelWall);

    Item *crowbar = new Item("crowbar", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 1, 1, itemScript, "crowbar");
    Item::Items.push_back(crowbar);

    Item *door = new Item("door", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 10, 1, itemScript, "door");
    Item::Items.push_back(door);

    Item *water = new Item("water", "Content/Textures/Tiles/dirt.png", false, BtnState::DOWN, 1, 1, itemScript, "water");
    Item::Items.push_back(water);

    Item *thruster = new Item("thruster", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 10, 1, itemScript, "thruster");
    Item::Items.push_back(thruster);

    Item *gridCutter = new Item("gridCutter", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 1, 2, itemScript, "gridCutter");
    Item::Items.push_back(gridCutter);

    Item *wirer = new Item("wirer", "Content/Textures/Tiles/dirt.png", false, BtnState::PRESSED, 1, 2, itemScript, "wirer");
    Item::Items.push_back(wirer);

    Item *light = new Item("light", "Content/Textures/Placeables/light.png", false, BtnState::PRESSED, 1, 1, itemScript, "light");
    Item::Items.push_back(light);

    Item *swtch = new Item("switch", "Content/Textures/Placeables/switch.png", false, BtnState::PRESSED, 1, 1, itemScript, "swtch");
    Item::Items.push_back(swtch);

    Item *o2 = new Item("o2", "Content/Textures/Placeables/o2.png", false, BtnState::PRESSED, 1, 1, itemScript, "o2");
    Item::Items.push_back(o2);

    Item *reactor = new Item("reactor", "Content/Textures/Placeables/reactor.png", false, BtnState::PRESSED, 1, 1, itemScript, "reactor");
    Item::Items.push_back(reactor);

    // Set up the placeables
    HSQUIRRELVM placeableScript = scriptSys->createScript("Content/Scripts/placeables.nut");
    PlaceableComponent::registerClass(placeableScript, "Door");
    PlaceableComponent::registerClass(placeableScript, "Thruster");
    PlaceableComponent::registerClass(placeableScript, "Light");
    PlaceableComponent::registerClass(placeableScript, "Swtch");

    // Set up the materials
    FrontGridComponent::addTileSheet(1, ResourceManager::get()->getTexture("Content/Textures/Tiles/dirt.png"));
    FrontGridComponent::addTileSheet(2, ResourceManager::get()->getTexture("Content/Textures/Tiles/stone.png"));
    FrontGridComponent::addTileSheet(3, ResourceManager::get()->getTexture("Content/Textures/Tiles/grass.png"));
    FrontGridComponent::addTileSheet(5, ResourceManager::get()->getTexture("Content/Textures/Tiles/steel.png"));
    FrontGridComponent::addTileSheet(6, ResourceManager::get()->getTexture("Content/Textures/Tiles/steel_window.png"));
    FrontGridComponent::addTileSheet(7, ResourceManager::get()->getTexture("Content/Textures/Tiles/glass.png"));

    BackGridComponent::addTileSheet(1, ResourceManager::get()->getTexture("Content/Textures/Tiles/dirt.png"));
    BackGridComponent::addTileSheet(2, ResourceManager::get()->getTexture("Content/Textures/Tiles/stone.png"));
    BackGridComponent::addTileSheet(3, ResourceManager::get()->getTexture("Content/Textures/Tiles/grass.png"));
    BackGridComponent::addTileSheet(5, ResourceManager::get()->getTexture("Content/Textures/Tiles/steel_wall.png"));
    BackGridComponent::addTileSheet(6, ResourceManager::get()->getTexture("Content/Textures/Tiles/steel_window.png"));
    BackGridComponent::addTileSheet(7, ResourceManager::get()->getTexture("Content/Textures/Tiles/glass.png"));

    gridSys->addTick(veggyGridOp, 5.f);
    gridSys->addTick(fluidGridOp, 0.001f);
    //gridSys->addTick(lightGridOp, 0.1f);

    network->login(user, pass);

    float accum = 0;
    int frames = 0;

    sf::Clock clock;
    while (render->getWindow().isOpen())
    {
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
            accum = 0;
            frames = 0;
        }
        frames++;

        conn->update(dt);
        engine->update(dt);
    }

    delete engine;
}

Tile** newWorld(int seed, int width, int height)
{
    int Mats = 7;
    srand(seed);
    int MC = rand()%Mats;
    int* mats = new int[MC];
    float** factors= new float* [MC];
    for(int i = 0; i < MC; i++){
        mats[i] = rand()%Mats;

        factors[i] = new float[3];
        factors[i][0] = 1/(float)((rand()%15)+1);
        factors[i][1] = 1/(float)((rand()%10)+1);
        factors[i][2] = 1/(float)((rand()%5)+1);
        std::cout << mats[i] <<" "<<factors[i][0]<<" "<< factors[i][1]<<" "<< factors[i][2] << std::endl;
    }

	Tile** tiles = new Tile*[height];
	for (int i = 0; i < height; i++)
		tiles[i] = new Tile[width];

	for (int y = 0; y < height; y++)
    {
         //std::cout << std::endl;
		for (int x = 0; x < width; x++)
		{
		    int mat = 0;
		    /*for(int m = 0; m < MC; m++){

                float f = PerlinNoise2D(y, x, 1+factors[m][0], factors[m][1], 11+factors[m][2]);
                if( y  < m*(height/MC)){
                   // v = f;
                    mat = y*(height/MC);
                  //  std::cout << f;
                }
		    }*/
		     mat = (MC*y)/height;

			auto n = PerlinNoise1D(x, 1.01, .02, 2) + 1;
            auto cave = PerlinNoise2D(y, x, 0.9, 0.15, 11)+1;
			//mat += float(height-y) / float(height);

            tiles[y][x].mLight = 20;

			if (y > n*100)
			{
			    if(cave < 0.1 || cave > 0.55){
                    tiles[y][x].mMat = mats[mat];
			    }
                tiles[y][x].mBack = 1;
                tiles[y][x].mLight = 0;

				if (tiles[y][x].mMat > 7)
					tiles[y][x].mMat = 3;


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
    transform.Func("move", (void (TransformComponent::*)(const sf::Vector2f&))&TransformComponent::move);
    transform.Func("setOrigin", (void (TransformComponent::*)(const sf::Vector2f&))&TransformComponent::setOrigin);
    Sqrat::RootTable(vm).Bind("TransformComponent", transform);

    Sqrat::DerivedClass<RenderComponent, Component> renderComp(vm);
    renderComp.Func("setLit", &RenderComponent::setLit);
    renderComp.Func("getLit", &RenderComponent::getLit);
    renderComp.Func("setLayer", &RenderComponent::setLayer);
    renderComp.Func("getLayer", &RenderComponent::getLayer);
    Sqrat::RootTable(vm).Bind("RenderComponent", renderComp);

    Sqrat::DerivedClass<WrapComponent, RenderComponent, sqext::ConstAlloc<WrapComponent, RenderComponent*>> wrapComp(vm);
    Sqrat::RootTable(vm).Bind("WrapComponent", wrapComp);

    Sqrat::DerivedClass<SpriteComponent, RenderComponent, sqext::ConstAlloc<SpriteComponent, const std::string&, int, int>> sprite(vm);
    //sprite.Func("setTexture", &SpriteComponent::setTexture);
    sprite.Func("setFrameLoop", &SpriteComponent::setFrameLoop);
    sprite.Func("setFrameDir", &SpriteComponent::setFrameDir);
    sprite.Func("setLoopAnim", &SpriteComponent::setLoopAnim);
    Sqrat::RootTable(vm).Bind("SpriteComponent", sprite);

    Sqrat::DerivedClass<IntentComponent, Component, sqext::ConstAlloc<IntentComponent>> intent(vm);
    intent.Func("mapKeyToIntent", &IntentComponent::mapKeyToIntent);
    intent.Func("mapMouseBtnToEvent", &IntentComponent::mapMouseBtnToIntent);
    intent.Func("isIntentActive", &IntentComponent::isIntentActive);
    intent.Func("getMousePos", &IntentComponent::getMousePos);
    Sqrat::RootTable(vm).Bind("IntentComponent", intent);

    Sqrat::DerivedClass<PlaceableComponent, Component, sqext::ConstAlloc<PlaceableComponent, Entity*, Entity*, const std::string&, int, int, int, int>> placeable(vm);
    Sqrat::RootTable(vm).Bind("PlaceableComponent", placeable);

    Sqrat::Class<Tile> tile(vm);
    tile.Var("mMat", &Tile::mMat);
    tile.Var("mBack", &Tile::mBack);
    tile.Var("mFluid", &Tile::mFluid);
    tile.Func("setSolid", &Tile::setSolid);
    tile.Func("clearSolid", &Tile::clearSolid);
    tile.Func("isSolid", &Tile::isSolid);
    Sqrat::RootTable(vm).Bind("Tile", tile);

    Sqrat::DerivedClass<GridComponent, Component> grid(vm);
    grid.Func("canPlace", &GridComponent::canPlace);
    grid.Func("addPlaceable", &GridComponent::addPlaceable);
    grid.Func("removePlaceable", &GridComponent::removePlaceable);
    grid.Func("getPlaceableAt", &GridComponent::getPlaceableAt);
    grid.Func("placeMid", &GridComponent::placeMid);
    grid.Func("placeBack", &GridComponent::placeBack);
    grid.Func("setSolid", &GridComponent::setSolid);
    grid.Func("setTile", &GridComponent::setTile);
    grid.Func("getTile", &GridComponent::getTile);
    grid.Func("sliceInto", &GridComponent::sliceInto);
    Sqrat::RootTable(vm).Bind("GridComponent", grid);

    Sqrat::Class<phys::RigidBody, sqext::ConstAlloc<phys::RigidBody, phys::Shape*, int, float>> rigidbody(vm);
    rigidbody.Func("applyForce", &phys::RigidBody::applyForce);

    rigidbody.Func("setVelocity", &phys::RigidBody::setVelocity);
    rigidbody.Func("setVelocityX", &phys::RigidBody::setVelocityX);
    rigidbody.Func("setVelocityY", &phys::RigidBody::setVelocityY);
    rigidbody.Func("getVelocity", &phys::RigidBody::getVelocity);
    Sqrat::RootTable(vm).Bind("RigidBody", rigidbody);

    Sqrat::DerivedClass<PhysicsComponent, Component, sqext::ConstAlloc<PhysicsComponent, float, float, float>> physics(vm);
    physics.Func("getBody", &PhysicsComponent::getBody);
    Sqrat::RootTable(vm).Bind("PhysicsComponent", physics);

    Sqrat::DerivedClass<PlayerComponent, Component, sqext::ConstAlloc<PlayerComponent>> player(vm);
    player.Func("getLeftCoordsCount", &PlayerComponent::getLeftCoordsCount);
    player.Func("pushLeftCoord", &PlayerComponent::pushLeftCoord);
    player.Func("frontLeftCoord", &PlayerComponent::frontLeftCoord);
    player.Func("popLeftCoord", &PlayerComponent::popLeftCoord);
    player.Func("getRightCoordsCount", &PlayerComponent::getRightCoordsCount);
    player.Func("pushRightCoord", &PlayerComponent::pushRightCoord);
    player.Func("frontRightCoord", &PlayerComponent::frontRightCoord);
    player.Func("popRightCoord", &PlayerComponent::popRightCoord);
    Sqrat::RootTable(vm).Bind("PlayerComponent", player);

    Sqrat::DerivedClass<LightComponent, Component, sqext::ConstAlloc<LightComponent, float>> light(vm);
    light.Func("setActive", &LightComponent::setActive);
    light.Func("getActive", &LightComponent::getActive);
    Sqrat::RootTable(vm).Bind("LightComponent", light);

    Sqrat::DerivedClass<SignalComponent, Component, sqext::ConstAlloc<SignalComponent>> signal(vm);
    signal.Func("addOutput", &SignalComponent::addOutput);
    signal.Func("fireInt", &SignalComponent::fireInt);
    signal.Func("fireFloat", &SignalComponent::fireFloat);
    signal.Func("fireVector", &SignalComponent::fireVector);
    signal.Func("hasSignal", &SignalComponent::hasSignal);
    signal.Func("getInt", &SignalComponent::getInt);
    signal.Func("getFloat", &SignalComponent::getFloat);
    signal.Func("getVector", &SignalComponent::getVector);
    Sqrat::RootTable(vm).Bind("SignalComponent", signal);

    Sqrat::DerivedClass<WrapComponent, Component, sqext::ConstAlloc<WrapComponent, RenderComponent*>> wrap(vm);
    Sqrat::RootTable(vm).Bind("WrapComponent", wrap);

    // Button states
    Sqrat::RootTable(vm).SetValue("BtnStateDown", BtnState::DOWN);
    Sqrat::RootTable(vm).SetValue("BtnStateUp", BtnState::UP);
    Sqrat::RootTable(vm).SetValue("BtnStatePressed", BtnState::PRESSED);
    Sqrat::RootTable(vm).SetValue("BtnStateReleased", BtnState::RELEASED);

    // Keyboard keys
    Sqrat::RootTable(vm).SetValue("KeyW", sf::Keyboard::W);
    Sqrat::RootTable(vm).SetValue("KeyS", sf::Keyboard::S);
    Sqrat::RootTable(vm).SetValue("KeyA", sf::Keyboard::A);
    Sqrat::RootTable(vm).SetValue("KeyD", sf::Keyboard::D);
    Sqrat::RootTable(vm).SetValue("KeyI", sf::Keyboard::I);
    Sqrat::RootTable(vm).SetValue("KeyK", sf::Keyboard::K);
    Sqrat::RootTable(vm).SetValue("KeyJ", sf::Keyboard::J);
    Sqrat::RootTable(vm).SetValue("KeyL", sf::Keyboard::L);

    // All the component types
    Sqrat::RootTable(vm).SetValue("TransformComponentType", TransformComponent::Type);
    Sqrat::RootTable(vm).SetValue("SpriteComponentType", SpriteComponent::Type);
    Sqrat::RootTable(vm).SetValue("IntentComponentType", IntentComponent::Type);
    Sqrat::RootTable(vm).SetValue("PlaceableComponentType", PlaceableComponent::Type);
    Sqrat::RootTable(vm).SetValue("GridComponentType", GridComponent::Type);
    Sqrat::RootTable(vm).SetValue("PhysicsComponentType", PhysicsComponent::Type);
    Sqrat::RootTable(vm).SetValue("LightComponentType", LightComponent::Type);
    Sqrat::RootTable(vm).SetValue("SignalComponentType", SignalComponent::Type);

    // All the component casting functions
    Sqrat::RootTable(vm).Func("castTransformComponent", componentCast<TransformComponent>);
    Sqrat::RootTable(vm).Func("castSpriteComponent", componentCast<SpriteComponent>);
    Sqrat::RootTable(vm).Func("castIntentComponent", componentCast<IntentComponent>);
    Sqrat::RootTable(vm).Func("castPlaceableComponent", componentCast<PlaceableComponent>);
    Sqrat::RootTable(vm).Func("castGridComponent", componentCast<GridComponent>);
    Sqrat::RootTable(vm).Func("castPhysicsComponent", componentCast<PhysicsComponent>);
    Sqrat::RootTable(vm).Func("castLightComponent", componentCast<LightComponent>);
    Sqrat::RootTable(vm).Func("castSignalComponent", componentCast<SignalComponent>);
}
