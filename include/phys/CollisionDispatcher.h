#ifndef COLLISIONDISPATCHER_H
#define COLLISIONDISPATCHER_H

#include <vector>
#include "phys/Shape.h"

namespace phys
{
    class Collision;
    class RigidBody;

    typedef void (*CollisionCallback)(Collision* c, RigidBody* a, RigidBody* b);

    class CollisionDispatcher
    {
        public:
            static void registerCallback(int a, int b, CollisionCallback callback);
            static void dispatch(Collision* c, RigidBody* a, RigidBody* b);

        private:
            static std::vector<std::vector<CollisionCallback>> callbacks;
    };
}


#endif // COLLISIONDISPATCHER_H
