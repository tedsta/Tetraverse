#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <Fission/Core/System.h>

class GridSystem;

class PhysicsSystem : public System
{
    public:
        PhysicsSystem(EventManager *eventManager, GridSystem* gridSys);
        virtual ~PhysicsSystem();

    protected:
        void begin(const float dt);

        void processEntity(Entity *entity, const float dt);

        void end(const float dt);

    private:
        GridSystem* mGridSys;
};

#endif // PHYSICSSYSTEM_H
