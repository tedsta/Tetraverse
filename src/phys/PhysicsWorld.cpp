#include "phys/PhysicsWorld.h"

#include <SFML/Graphics/Rect.hpp>

#include "phys/RigidBody.h"
#include "phys/Constraint.h"
#include "phys/Shape.h"

namespace phys
{
    void PhysicsWorld::integrateForces( RigidBody *b, float dt )
    {
        if(b->inverseMass == 0.0f)
            return;

        b->velocity += (b->force * b->inverseMass + b->gravity) * (dt / 2.0f);
        b->angularVelocity += b->torque * b->inverseInertia * (dt / 2.0f);
    }

    void PhysicsWorld::integrateVelocity( RigidBody *b, float dt )
    {
        if(b->inverseMass == 0.0f)
            return;

        b->position += b->velocity * dt;
        b->rotation += b->angularVelocity * dt;
        b->setRotation( b->rotation );
        integrateForces( b, dt );
    }

    PhysicsWorld::PhysicsWorld(float _dt, sf::Uint32 _iterations) : dt(_dt), iterations(_iterations)
    {
        //ctor
    }

    PhysicsWorld::~PhysicsWorld()
    {
        //dtor
    }

    void PhysicsWorld::step()
    {
        // Generate new collision info
        contacts.clear();
        for(sf::Uint32 i = 0; i < bodies.size(); ++i)
        {
            RigidBody* A = bodies[i];
            sf::FloatRect ABounds = A->getShape()->getLocalBounds();
            ABounds.left += A->position.x;
            ABounds.top += A->position.y;

            A->oldPosition = A->position;

            for(sf::Uint32 j = i + 1; j < bodies.size(); ++j)
            {
                RigidBody* B = bodies[j];

                sf::FloatRect BBounds = B->getShape()->getLocalBounds();
                BBounds.left += B->position.x;
                BBounds.top += B->position.y;

                if (!ABounds.intersects(BBounds))
                    continue;

                if(A->inverseMass == 0 && B->inverseMass == 0)
                    continue;
                Collision c(A, B, dt, sf::Vector2f(0, 40));
                c.solve();
                if(c.hasCollision())
                {
                    contacts.emplace_back(c);
                }
            }
        }

        // Integrate forces
        for(auto b : bodies)
            integrateForces(b, dt);

        // Initialize collision
        for(auto& contact : contacts)
            contact.initialize();

        // Solve collisions
        for(sf::Uint32 j = 0; j < iterations; ++j)
            for(auto& contact : contacts)
                contact.applyImpulse();

        // Solve constraints
        for (auto constraint : constraints)
            constraint->solve(dt);

        // Integrate velocities
        for(auto b : bodies)
            integrateVelocity(b, dt);

        // Correct positions
        for(auto& contact : contacts)
            contact.positionalCorrection();

        // Clear all forces
        for(auto b : bodies)
        {
            b->force.x = b->force.y = 0;
            b->torque = 0;
        }
    }

    void PhysicsWorld::removeRigidBody(RigidBody* body)
    {
        for (sf::Uint32 i = 0; i < bodies.size(); i++)
            if (bodies[i] == body)
            {
                bodies.erase(bodies.begin()+i);
                return;
            }
    }
}

