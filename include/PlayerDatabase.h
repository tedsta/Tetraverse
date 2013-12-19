#ifndef PLAYERDATABASE_H
#define PLAYERDATABASE_H

#include <string>
#include <vector>

class Engine;
class Entity;

struct Player
{
    std::string mName;
    std::string mPassword;
    int mNetID; // -1 if the player isn't logged in
    Entity* mEntity;
};

class PlayerDatabase
{
    public:
        PlayerDatabase(Engine* engine);
        virtual ~PlayerDatabase();

        void createPlayer(std::string name, std::string password);
        bool validatePlayer(std::string name, std::string password);
        bool loginPlayer(std::string name, int netID);
        void logoutPlayer(int netID);

        Player* findPlayer(std::string name);
        Player* findPlayer(int netID);

    private:
        Engine* mEngine;

        std::vector<Player> mPlayers;
};

#endif // PLAYERDATABASE_H
