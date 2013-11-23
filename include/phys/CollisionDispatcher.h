#ifndef COLLISIONDISPATCHER_H
#define COLLISIONDISPATCHER_H

#include <vector>
#include "phys/Shape.h"

namespace phys
{
    class Manifold;
    class RigidBody;

    typedef void (*CollisionCallback)(Manifold* m, RigidBody* a, RigidBody* b);

    class CollisionDispatcher
    {
        public:
            static void registerCallback(int a, int b, CollisionCallback callback);
            static void dispatch(Manifold* m, RigidBody* a, RigidBody* b);

        private:
            static std::vector<std::vector<CollisionCallback>> callbacks;
    };
}


#endif // COLLISIONDISPATCHER_H
