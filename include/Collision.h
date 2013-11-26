#ifndef COLLISION_H
#define COLLISION_H

#include "phys/Manifold.h"
#include "phys/RigidBody.h"

void gridToPolygon(phys::Manifold *m, phys::RigidBody *a, phys::RigidBody *b);

#endif // COLLISION_H
