#include "TVNetwork.h"

#include "PlayerDatabase.h"

TVNetwork::TVNetwork(Connection* conn, PlayerDatabase* playerDB) : mConnection(conn), mPlayerDB(playerDB)
{
    //ctor
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

            if (!mPlayerDB->validatePlayer(username, password))
                break;

            mPlayerDB->loginPlayer(username, netID);

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
        case 0:
            break;
        }
    }
}
