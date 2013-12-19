#include "PlayerDatabase.h"

#include <SFML/Window/Event.hpp>

#include <Fission/Core/Engine.h>
#include <Fission/Core/Scene.h>
#include <Fission/Core/Entity.h>

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

PlayerDatabase::PlayerDatabase(Engine* engine) : mEngine(engine)
{
    //ctor
}

PlayerDatabase::~PlayerDatabase()
{
    //dtor
}

void PlayerDatabase::createPlayer(std::string name, std::string password)
{
    Player player;
    player.mName = name;
    player.mPassword = password;
    player.mNetID = -1;

    player.mEntity = new Entity(mEngine->getEventManager());
    player.mEntity->giveID();
    player.mEntity->addComponent(new TransformComponent(sf::Vector2f(100, 1000)));
    player.mEntity->addComponent(new SkeletonComponent("Content/Spine/player.mEntity.json", "Content/Spine/player.mEntity.atlas"));
    player.mEntity->addComponent(new IntentComponent);
    player.mEntity->addComponent(new PhysicsComponent(1.5f, 1.f));
    player.mEntity->addComponent(new PlayerComponent);
    InventoryComponent* inventory = new InventoryComponent(10);
    player.mEntity->addComponent(inventory);

    reinterpret_cast<SkeletonComponent*>(player.mEntity->getComponent(SkeletonComponent::Type))->setLayer(3);

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

    TransformComponent *trans = static_cast<TransformComponent*>(player.mEntity->getComponent(TransformComponent::Type));
    IntentComponent *intent = static_cast<IntentComponent*>(player.mEntity->getComponent(IntentComponent::Type));

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

    mPlayers.push_back(player);
}

bool PlayerDatabase::validatePlayer(std::string name, std::string password)
{
    Player* player = findPlayer(name);

    if (!player || player->mNetID != -1 || player->mPassword != password)
        return false;

    return true;
}

bool PlayerDatabase::loginPlayer(std::string name, int netID)
{
    Player* player = findPlayer(name);

    if (!player || player->mNetID != -1)
        return false;

    mEngine->getScene()->addEntity(player->mEntity);
    player->mNetID = netID;

    return true;
}

void PlayerDatabase::logoutPlayer(int netID)
{
    Player* player = findPlayer(netID);

    if (!player)
        return;

    mEngine->getScene()->removeEntity(player->mEntity);
    player->mNetID = -1;
}

Player* PlayerDatabase::findPlayer(std::string name)
{
    for (auto& player : mPlayers)
    {
        if (player.mName == name)
            return &player;
    }

    return NULL;
}

Player* PlayerDatabase::findPlayer(int netID)
{
    if (netID < 0)
        return NULL;

    for (auto& player : mPlayers)
    {
        if (player.mNetID == netID)
            return &player;
    }

    return NULL;
}
