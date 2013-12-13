

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

#include "GridSystem.h"
#include "PlaceableSystem.h"
#include "PhysicsSystem.h"
#include "PlayerSystem.h"
#include "LightSystem.h"

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

    Connection* conn = new Connection(engine->getEventManager());

    RenderSystem *render = new RenderSystem(engine->getEventManager(), 0.f, ResourceManager::get()->getFont("Content/Fonts/font.ttf"),
                                            BackGridComponent::Type|FrontGridComponent::Type|SkeletonComponent::Type);
    InputSystem *input = new InputSystem(engine->getEventManager(), 1.f/30.f, &render->getWindow());
    IntentSystem *intentSys = new IntentSystem(engine->getEventManager(), 1.f/30.f, conn);
    ScriptSystem *scriptSys = new ScriptSystem(engine->getEventManager(), 1.f/30.f, engine);
    PhysicsSystem *physSys = new PhysicsSystem(engine->getEventManager(), 1.f/30.f);
    GridSystem *gridSys = new GridSystem(engine->getEventManager(), physSys, 1.f/1000.f);
    PlaceableSystem *placeableSys = new PlaceableSystem(engine->getEventManager(), 1.f/30.f);
    PlayerSystem *playerSys = new PlayerSystem(engine->getEventManager(), render, 1.f/30.f);
    LightSystem* lightSys = new LightSystem(engine->getEventManager(), render, 0.f);

    engine->addSystem(render);
    engine->addSystem(input);
    engine->addSystem(intentSys);
    engine->addSystem(scriptSys);
    engine->addSystem(playerSys);
    engine->addSystem(placeableSys);
    engine->addSystem(physSys);
    engine->addSystem(gridSys);
    //engine->addSystem(lightSys);

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
    gridSys->addTick(lightGridOp, 0.1f);

    Scene *scene = engine->getScene();

    int worldW = 200;
    int worldH = 200;
    Tile** tiles = newWorld(0, worldW, worldH);

    TransformComponent* pt = new TransformComponent(sf::Vector2f(0, 0));
    GridComponent* pg = new GridComponent(pt, worldW, worldH, true, tiles, 3);

    Entity *planet = new Entity(engine->getEventManager());
    scene->addEntity(planet);
    planet->addComponent(pt);
    planet->addComponent(pg);
    planet->addComponent(new BackGridComponent(pg));
    planet->addComponent(new FrontGridComponent(pg));
    planet->addComponent(new PhysicsComponent(pg));

    // Spawn player
    std::vector<sf::Vector2f> playerVerts(8);
    playerVerts[0] = sf::Vector2f(-0.5f, -1.8f);
    playerVerts[1] = sf::Vector2f(0.5f, -1.8f);
    playerVerts[2] = sf::Vector2f(0.9f, -1.5f);
    playerVerts[3] = sf::Vector2f(0.9f, 1.5f);
    playerVerts[4] = sf::Vector2f(0.5f, 1.8f);
    playerVerts[5] = sf::Vector2f(-0.5f, 1.8f);
    playerVerts[6] = sf::Vector2f(-0.9f, 1.5f);
    playerVerts[7] = sf::Vector2f(-0.9f, -1.5f);

    Entity *player = new Entity(engine->getEventManager());
    scene->addEntity(player);
    player->addComponent(new TransformComponent(sf::Vector2f(100, 1000)));
    //player->addComponent(new SpriteComponent("robot.png"));
    player->addComponent(new SkeletonComponent("Content/Spine/player.json", "Content/Spine/player.atlas"));
    player->addComponent(new IntentComponent);
    player->addComponent(new PhysicsComponent(playerVerts.data(), playerVerts.size()));
    player->addComponent(new PlayerComponent);
    player->addComponent(new LightComponent(500.f));
    InventoryComponent* inventory = new InventoryComponent(10);
    player->addComponent(inventory);

    reinterpret_cast<SkeletonComponent*>(player->getComponent(SkeletonComponent::Type))->setLayer(3);

    inventory->addItem(0, 3, 1);
    inventory->addItem(1, 1, 999);
    inventory->addItem(2, 2, 999);
    inventory->addItem(3, 4, 1);
    inventory->addItem(4, 6, 1);
    inventory->addItem(5, 7, 1);
    inventory->addItem(6, 11, 1);
    inventory->addItem(7, 9, 99);
    inventory->addItem(8, 10, 99);
    inventory->addItem(9, 12, 1);

    TransformComponent *trans = static_cast<TransformComponent*>(player->getComponent(TransformComponent::Type));
    IntentComponent *intent = static_cast<IntentComponent*>(player->getComponent(IntentComponent::Type));

    //trans->setOrigin(sf::Vector2f(30, 48));
    //trans->setPosition(sf::Vector2f(-worldW*TILE_SIZE, -worldH*TILE_SIZE)/2.f + sf::Vector2f(1000, 1000));
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

    intent->mapMouseBtnToIntent("useLeft", sf::Mouse::Button::Left, BtnState::PRESSED);
    intent->mapMouseBtnToIntent("useLeftRelease", sf::Mouse::Button::Left, BtnState::RELEASED);
    intent->mapMouseBtnToIntent("useRight", sf::Mouse::Button::Right, BtnState::PRESSED);
    intent->mapMouseBtnToIntent("useRightRelease", sf::Mouse::Button::Right, BtnState::RELEASED);
    intent->mapKeyToIntent("interact", sf::Keyboard::E, BtnState::PRESSED);
    intent->mapKeyToIntent("test", sf::Keyboard::T, BtnState::PRESSED);

    intent->mapKeyToIntent("zoomout", sf::Keyboard::Up, BtnState::DOWN);
    intent->mapKeyToIntent("zoomin", sf::Keyboard::Down, BtnState::DOWN);

    intent->mapKeyToIntent("stupidmode", sf::Keyboard::Space, BtnState::PRESSED);

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

            tiles[y][x].mLight = 20;

			if (y > n*100)
			{
                tiles[y][x].mMat = p * 3 / 2;
                tiles[y][x].mBack = 1;
                tiles[y][x].mLight = 0;

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
    Sqrat::DerivedClass<RenderComponent, Component> renderComp(vm);
    renderComp.Func("setLit", &RenderComponent::setLit);
    renderComp.Func("getLit", &RenderComponent::getLit);
    renderComp.Func("setLayer", &RenderComponent::setLayer);
    renderComp.Func("getLayer", &RenderComponent::getLayer);
    Sqrat::RootTable(vm).Bind("RenderComponent", renderComp);

    Sqrat::DerivedClass<TransformComponent, Component, sqext::ConstAlloc<TransformComponent, sf::Vector2f, float, sf::Vector2f>> transform(vm);
    transform.Func("setPosition", (void (TransformComponent::*)(const sf::Vector2f&))&TransformComponent::setPosition);
    transform.Func("getPosition", &TransformComponent::getPosition);
    transform.Func("move", (void (TransformComponent::*)(const sf::Vector2f&))&TransformComponent::move);
    transform.Func("setOrigin", (void (TransformComponent::*)(const sf::Vector2f&))&TransformComponent::setOrigin);
    Sqrat::RootTable(vm).Bind("TransformComponent", transform);

    Sqrat::DerivedClass<SpriteComponent, RenderComponent, sqext::ConstAlloc<SpriteComponent, const std::string&, int, int>> sprite(vm);
    //sprite.Func("setTexture", &SpriteComponent::setTexture);
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
    Sqrat::RootTable(vm).Bind("Tile", tile);

    Sqrat::DerivedClass<GridComponent, Component> grid(vm);
    grid.Func("canPlace", &GridComponent::canPlace);
    grid.Func("addPlaceable", &GridComponent::addPlaceable);
    grid.Func("removePlaceable", &GridComponent::removePlaceable);
    grid.Func("getPlaceableAt", &GridComponent::getPlaceableAt);
    grid.Func("placeMid", &GridComponent::placeMid);
    grid.Func("placeBack", &GridComponent::placeBack);
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
