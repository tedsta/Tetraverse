#include "phys/PhysicsWorld.h"

#include "phys/RigidBody.h"
#include "phys/Constraint.h"

namespace phys
{
    const sf::Vector2f gravity = sf::Vector2f(0, 40.f);

    void PhysicsWorld::integrateForces( RigidBody *b, float dt )
    {
        if(b->inverseMass == 0.0f)
            return;

        b->velocity += (b->force * b->inverseMass + gravity) * (dt / 2.0f);
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

            for(sf::Uint32 j = i + 1; j < bodies.size(); ++j)
            {
                RigidBody* B = bodies[j];
                if(A->inverseMass == 0 && B->inverseMass == 0)
                    continue;
                Manifold m(A, B, dt, gravity);
                m.solve();
                if(m.contactCount)
                    contacts.emplace_back(m);
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

