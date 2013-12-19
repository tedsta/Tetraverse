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
    bool mLoggedIn;
    Entity* mEntity;
};

class PlayerDatabase
{
    public:
        PlayerDatabase(Engine* engine);
        virtual ~PlayerDatabase();

        void createPlayer(std::string name, std::string password);
        bool loginPlayer(std::string name, std::string password);
        void logoutPlayer(std::string name);

        Player* findPlayer(std::string name);

    private:
        Engine* mEngine;

        std::vector<Player> mPlayers;
};

#endif // PLAYERDATABASE_H
