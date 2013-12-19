#ifndef TVNETWORK_H
#define TVNETWORK_H

#include <Fission/Network/Connection.h>

class PlayerDatabase;

// Packet types
enum
{
    LOGIN,
    LOGOUT
};

class TVNetwork : public IPacketHandler
{
    public:
        TVNetwork(Connection* conn, PlayerDatabase* playerDB);
        virtual ~TVNetwork();

        void handlePacket(sf::Packet& packet, int netID);

    private:
        Connection* mConnection;
        PlayerDatabase* mPlayerDB;
};

#endif // TVNETWORK_H
