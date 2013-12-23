#include "TVNetwork.h"

#include <Fission/Core/Engine.h>
#include <Fission/Core/Scene.h>
#include <Fission/Core/Entity.h>

#include "PlayerDatabase.h"

TVNetwork::TVNetwork(Engine *engine, Connection* conn, PlayerDatabase* playerDB) : mEngine(engine), mConnection(conn), mPlayerDB(playerDB)
{
    mConnection->registerHandlerAuto(this);
}

TVNetwork::~TVNetwork()
{
    //dtor
}

void TVNetwork::handlePacket(sf::Packet& packet, int netID)
{
    int packetID;
    packet >> packetID;

    if (mConnection->getType() == NetType::SERVER)
    {
        switch (packetID)
        {
        case LOGIN:
        {
            std::string username, password;
            packet >> username >> password;

            mPlayerDB->createPlayer(username, password);

            if (!mPlayerDB->validatePlayer(username, password))
                break;

            sendScene(netID);

            if (!mPlayerDB->loginPlayer(username, netID))
                break;

            sendEntity(mPlayerDB->findPlayer(username)->mEntity);

            std::cout << username << " logged in.\n";

            break;
        }

        case LOGOUT:
        {
            mPlayerDB->logoutPlayer(netID);

            break;
        }
        }
    }
    else if (mConnection->getType() == NetType::CLIENT)
    {
        switch (packetID)
        {
        case CREATE_ENTITY:
        {
            Entity* entity = mEngine->getScene()->createEntity();
            entity->deserialize(packet);
            entity->postDeserialize();
            break;
        }

        case CREATE_SCENE:
            mEngine->getScene()->deserialize(packet);
            break;
        }
    }
}

void TVNetwork::login(std::string user, std::string pass)
{
    sf::Packet packet;
    packet << int(LOGIN);
    packet << user << pass;
    mConnection->send(packet, getHndID());
}

void TVNetwork::sendEntity(Entity* entity, int netID, int excludeID)
{
    sf::Packet packet;
    packet << int(CREATE_ENTITY);
    entity->serialize(packet);
    mConnection->send(packet, getHndID(), netID, excludeID);
}

void TVNetwork::sendScene(int netID, int excludeID)
{
    sf::Packet packet;
    packet << int(CREATE_SCENE);
    mEngine->getScene()->serialize(packet);
    mConnection->send(packet, getHndID(), netID, excludeID);
}
