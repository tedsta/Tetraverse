#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <Fission/Core/System.h>

#include "phys/PhysicsWorld.h"

class GridSystem;
class PhysicsComponent;

class PhysicsSystem : public System
{
    public:
        PhysicsSystem(EventManager *eventManager, float lockStep);
        virtual ~PhysicsSystem();

        phys::PhysicsWorld* getWorld(){return world;}

    protected:
        void begin(const float dt);

        void processEntity(Entity *entity, const float dt);

        void end(const float dt);

    private:
        phys::PhysicsWorld* world;
};

#endif // PHYSICSSYSTEM_H
