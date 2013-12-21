#ifndef EMITTERSYSTEM_H
#define EMITTERSYSTEM_H

#include <Fission/Core/System.h>

class EmitterSystem : public System
{
    public:
        EmitterSystem(EventManager *eventManager, float lockStep);
        virtual ~EmitterSystem();

        void begin(const float dt);

        void processEntity(Entity *entity, const float dt);

        void end(const float dt);

    private:

};

#endif // EMITTERSYSTEM_H
