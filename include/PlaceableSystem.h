#ifndef PLACEABLESYSTEM_H
#define PLACEABLESYSTEM_H

#include <Fission/Core/System.h>

class PlaceableSystem : public System
{
    public:
        PlaceableSystem(EventManager *eventManager, float lockStep);
        virtual ~PlaceableSystem();
    protected:
        void begin(const float dt);

        void processEntity(Entity *entity, const float dt);

        void end(const float dt);

    private:
};

#endif // PLACEABLESYSTEM_H
