#include "phys/PhysicsWorld.h"

#include <stack>
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
        int typeCount = 0;
        for (auto b : bodies)
        {
            if (b->getType() > typeCount)
                typeCount = b->getType();
        }

        for (int curType = 0; curType <= typeCount; curType++)
        {
            for (auto b : bodies)
            {
                if (b->getType() >= curType)
                    continue;

                b->velocity_ = b->velocity;
                b->velocity.x = b->velocity.y = 0.f;
                b->angularVelocity_ = b->angularVelocity;
                b->angularVelocity = 0.f;
                b->mass = 0.f;
                b->inverseMass = 0.f;
                b->inertia = 0.f;
                b->inverseInertia = 0.f;
            }

            // Generate new collision info
            contacts.clear();
            for(sf::Uint32 i = 0; i < bodies.size(); ++i)
            {
                if (bodies[i]->getType() > curType)
                    continue;

                RigidBody* A = bodies[i];
                sf::FloatRect ABounds = A->getShape()->getLocalBounds();
                ABounds.left += A->position.x;
                ABounds.top += A->position.y;

                A->oldPosition = A->position;

                for(sf::Uint32 j = i + 1; j < bodies.size(); ++j)
                {
                    if (bodies[j]->getType() > curType)
                        continue;

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
            {
                if (b->getType() > curType)
                    continue;

                integrateForces(b, dt);
            }

            // Initialize collision
            for(auto& contact : contacts)
            {
                contact.initialize();
            }

            // Solve collisions
            for(sf::Uint32 j = 0; j < iterations; ++j)
                for(auto& contact : contacts)
                    contact.applyImpulse();

            // Solve constraints
            for (auto constraint : constraints)
                constraint->solve(dt);

            // Integrate velocities
            for(auto b : bodies)
            {
                if (b->getType() > curType)
                    continue;

                integrateVelocity(b, dt);
            }

            // Correct positions
            for(auto& contact : contacts)
                contact.positionalCorrection();

            // Clear all forces
            for(auto b : bodies)
            {
                if (b->getType() > curType)
                    continue;

                b->force.x = b->force.y = 0;
                b->torque = 0;
            }

            for(auto b : bodies)
            {
                if (b->getType() >= curType)
                    continue;

                b->velocity = b->velocity_;
                b->angularVelocity = b->angularVelocity_;
                b->mass = b->mass_;
                b->inverseMass = b->inverseMass_;
                b->inertia = b->inertia_;
                b->inverseInertia = b->inverseInertia_;
            }

            for (auto b : bodies)
            {
                if (b->getType() != curType)
                    continue;

                std::stack<RigidBody*> pstack;

                for (auto child : b->children)
                    pstack.push(child);

                while (!pstack.empty())
                {
                    RigidBody* current = pstack.top();
                    pstack.pop();
                    current->position += b->position-b->oldPosition;

                    for (auto child : current->children)
                        pstack.push(child);
                }
            }
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

