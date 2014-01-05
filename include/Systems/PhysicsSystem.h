#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <Fission/Core/System.h>

#include "phys/PhysicsWorld.h"

class GridSystem;
class RigidBody;

class PhysicsSystem : public fsn::System
{
    public:
        PhysicsSystem(fsn::IEventManager* eventManager, float lockStep);
        virtual ~PhysicsSystem();

        phys::PhysicsWorld* getWorld(){return mWorld;}

    protected:
        void begin(const float dt);

        void processEntity(fsn::EntityRef* entity, const float dt);

        void end(const float dt);

        void onEntityAdded(fsn::EntityRef* entity);
        void onEntityRemoved(fsn::EntityRef* entity);

    private:
        phys::PhysicsWorld* mWorld;
};

#endif // PHYSICSSYSTEM_H
