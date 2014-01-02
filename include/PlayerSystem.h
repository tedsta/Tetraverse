#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H

#include <Fission/Core/System.h>

class RenderSystem;
class Connection;

class PlayerSystem : public System
{
    public:
        PlayerSystem(EventManager *eventManager, RenderSystem* rndSys, Connection* conn, float lockStep);
        virtual ~PlayerSystem();

    protected:
        void begin(const float dt);

        void processEntity(Entity *entity, const float dt);

        void end(const float dt);

    private:
        RenderSystem* mRndSys;
        Connection* mConnection;
};

#endif // PLAYERSYSTEM_H
