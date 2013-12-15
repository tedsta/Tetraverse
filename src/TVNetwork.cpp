#include "TVNetwork.h"

TVNetwork::TVNetwork()
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

            break;
        }

        case LOGOUT:
        {
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
