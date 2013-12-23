#ifndef TVNETWORK_H
#define TVNETWORK_H

#include <Fission/Network/Connection.h>

class PlayerDatabase;

// Packet types
enum
{
    LOGIN,
    LOGOUT,

    CREATE_ENTITY,
    CREATE_SCENE
};

class Engine;

class TVNetwork : public IPacketHandler
{
    public:
        TVNetwork(Engine* engine, Connection* conn, PlayerDatabase* playerDB);
        virtual ~TVNetwork();

        void handlePacket(sf::Packet& packet, int netID);

        // This is for clients
        void login(std::string user, std::string pass);

        // This is for servers
        void sendEntity(Entity* entity, int netID = 0, int excludeID = 0);
        void sendScene(int netID = 0, int excludeID = 0);

    private:
        Engine* mEngine;
        Connection* mConnection;
        PlayerDatabase* mPlayerDB;
};

#endif // TVNETWORK_H
