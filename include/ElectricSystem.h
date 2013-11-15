#ifndef ELECTEIXSYSTEM_H
#define ELECTRICSYSTEM_H

#include "Fission/Core/System.h"
#include "Fission/Network/Connection.h"
#include "GridComponent.h"

#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

// Grid operation function. The surrounding tiles and the physics ticks are passed

class ElectricSystem : public System
{
    public:
        ElectricSystem(EventManager *eventManager);
    protected:
        void begin(const float dt);

        void processEntity(Entity *entity, const float dt);

        void end(const float dt);

  //  private:
};

#endif // GRIDSYSTEM_H
