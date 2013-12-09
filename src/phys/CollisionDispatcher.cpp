#include "phys/CollisionDispatcher.h"

#include "phys/RigidBody.h"
#include "phys/Shape.h"

namespace phys
{
    std::vector<std::vector<CollisionCallback>> CollisionDispatcher::callbacks;

    void CollisionDispatcher::registerCallback(int a, int b, CollisionCallback callback)
    {
        if (int(callbacks.size()) <= a)
            callbacks.resize(a+1);
        if (int(callbacks[a].size()) <= b)
        {
            for (auto& row : callbacks)
                row.resize(b+1);
        }

        callbacks[a][b] = callback;
    }

    void CollisionDispatcher::dispatch(Collision* c, RigidBody* a, RigidBody* b)
    {
        callbacks[a->getShape()->getType()][b->getShape()->getType()](c, a, b);
    }
}

