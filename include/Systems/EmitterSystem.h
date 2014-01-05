#ifndef EMITTERSYSTEM_H
#define EMITTERSYSTEM_H

#include <Fission/Core/System.h>

class EmitterSystem : public fsn::System
{
    public:
        EmitterSystem(fsn::IEventManager* eventManager, float lockStep);
        virtual ~EmitterSystem();

        void begin(const float dt);

        void processEntity(fsn::EntityRef* entity, const float dt);

        void end(const float dt);

    private:

};

#endif // EMITTERSYSTEM_H
