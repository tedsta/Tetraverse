#ifndef TVNETWORK_H
#define TVNETWORK_H

#include <Fission/Network/Connection.h>

// Packet types
enum
{
    LOGIN,
    LOGOUT
};

class TVNetwork : public IPacketHandler
{
    public:
        TVNetwork();
        virtual ~TVNetwork();

        void handlePacket(sf::Packet& packet, int netID);

    private:
        Connection* mConnection;
};

#endif // TVNETWORK_H
