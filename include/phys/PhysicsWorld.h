#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <vector>
#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>

#include "phys/Manifold.h"

namespace phys
{
    class RigidBody;
    class Constraint;

    class PhysicsWorld
    {
        public:
            PhysicsWorld(float _dt, sf::Uint32 _iterations);
            virtual ~PhysicsWorld();

            void step();

            void addRigidBody(RigidBody* body){bodies.push_back(body);}
            void removeRigidBody(RigidBody* body);

            void addConstraint(Constraint* constraint){constraints.push_back(constraint);}

        private:
            void integrateForces( RigidBody *b, float dt );
            void integrateVelocity( RigidBody *b, float dt );

            float dt;
            sf::Uint32 iterations;
            std::vector<RigidBody*> bodies;
            std::vector<Constraint*> constraints;
            std::vector<Collision> contacts;
    };
}

#endif // PHYSICSWORLD_H
