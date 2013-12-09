#include "phys/Manifold.h"

#include <algorithm>

#include "phys/PhysMath.h"
#include "phys/RigidBody.h"
#include "phys/CollisionDispatcher.h"

namespace phys
{
    Manifold::Manifold(RigidBody* a, RigidBody* b, float _dt, const sf::Vector2f& _gravity) : bodyA(a), bodyB(b),
        dt(_dt), gravity(_gravity)
    {
        //ctor
    }

    Manifold::~Manifold()
    {
        //dtor
    }

    void Manifold::initialize()
    {
        // Calculate average restitution
        restitution = std::min( bodyA->restitution, bodyB->restitution );

        // Calculate static and dynamic friction
        staticFriction = std::sqrt( bodyA->staticFriction * bodyA->staticFriction );
        dynamicFriction = std::sqrt( bodyA->dynamicFriction * bodyA->dynamicFriction );

        for(sf::Uint32 i = 0; i < contactCount; ++i)
        {
            // Calculate radii from COM to contact
            sf::Vector2f ra = contacts[i] - bodyA->position;
            sf::Vector2f rb = contacts[i] - bodyB->position;

            sf::Vector2f rv = bodyB->velocity + cross( bodyB->angularVelocity, rb ) -
            bodyA->velocity - cross( bodyA->angularVelocity, ra );


            // Determine if we should perform a resting collision or not
            // The idea is if the only thing moving this object is gravity,
            // then the collision should be performed without any restitution
            if(lengthSqr(rv) < lengthSqr(dt * gravity) + EPSILON)
                restitution = 0.0f;
        }
    }

    void Manifold::applyImpulse()
    {
        // Early out and positional correct if both objects have infinite mass
        if(equal( bodyA->inverseMass + bodyB->inverseMass, 0 ))
        {
            infiniteMassCorrection( );
            return;
        }

        for(sf::Uint32 i = 0; i < contactCount; ++i)
        {
            // Calculate radii from COM to contact
            sf::Vector2f ra = contacts[i] - bodyA->position;
            sf::Vector2f rb = contacts[i] - bodyB->position;

            // Relative velocity
            sf::Vector2f rv = bodyB->velocity + cross( bodyB->angularVelocity, rb ) -
            bodyA->velocity - cross( bodyA->angularVelocity, ra );

            // Relative velocity along the normal
            float contactVel = dot( rv, normal );

            // Do not resolve if velocities are separating
            if(contactVel > 0)
                return;

            float raCrossN = cross( ra, normal );
            float rbCrossN = cross( rb, normal );
            float invMassSum = bodyA->inverseMass + bodyB->inverseMass + sqr( raCrossN ) * bodyA->inverseInertia + sqr( rbCrossN ) * bodyB->inverseInertia;

            // Calculate impulse scalar
            float j = -(1.0f + restitution) * contactVel;
            j /= invMassSum;
            j /= (float)contactCount;

            // Apply impulse
            sf::Vector2f impulse = normal * j;
            bodyA->applyImpulse( -impulse, ra );
            bodyB->applyImpulse(  impulse, rb );

            // Friction impulse
            rv = bodyB->velocity + cross( bodyB->angularVelocity, rb ) -
            bodyA->velocity - cross( bodyA->angularVelocity, ra );

            sf::Vector2f t = rv - (normal * dot( rv, normal ));
            t = normalize(t);

            // j tangent magnitude
            float jt = -dot( rv, t );
            jt /= invMassSum;
            jt /= (float)contactCount;

            // Don't apply tiny friction impulses
            if(equal( jt, 0.0f ))
                return;

            // Coulumb's law
            sf::Vector2f tangentImpulse;
            if(std::abs( jt ) < j * staticFriction)
                tangentImpulse = t * jt;
            else
                tangentImpulse = t * -j * dynamicFriction;

            // Apply friction impulse
            bodyA->applyImpulse( -tangentImpulse, ra );
            bodyB->applyImpulse(  tangentImpulse, rb );
        }
    }

    void Manifold::positionalCorrection()
    {
        const float k_slop = 0.05f; // Penetration allowance
        const float percent = 0.4f; // Penetration percentage to correct
        sf::Vector2f correction = (std::max( penetration - k_slop, 0.0f ) / (bodyA->inverseMass + bodyB->inverseMass)) * normal * percent;
        bodyA->position -= correction * bodyA->inverseMass;
        bodyB->position += correction * bodyB->inverseMass;
    }

    void Manifold::infiniteMassCorrection()
    {
        bodyA->velocity = sf::Vector2f(0, 0);
        bodyB->velocity = sf::Vector2f(0, 0);
    }

    // ********************************************************************************************

    Collision::Collision(RigidBody* a, RigidBody* b, float _dt, const sf::Vector2f& _gravity) : bodyA(a), bodyB(b),
        dt(_dt), gravity(_gravity), collision(false)
    {
        //ctor
    }

    void Collision::addManifold()
    {
        manifolds.emplace_back(Manifold(bodyA, bodyB, dt, gravity));
    }

    void Collision::solve()
    {
        CollisionDispatcher::dispatch(this, bodyA, bodyB);
        for (auto& manifold : manifolds)
            if (manifold.contactCount)
                collision = true;
    }

    void Collision::initialize()
    {
        for (auto& manifold : manifolds)
            if (manifold.contactCount)
                manifold.initialize();
    }

    void Collision::applyImpulse()
    {
        for (auto& manifold : manifolds)
            if (manifold.contactCount)
                manifold.applyImpulse();
    }

    void Collision::positionalCorrection()
    {
        for (auto& manifold : manifolds)
            if (manifold.contactCount)
                manifold.positionalCorrection();
    }

    void Collision::infiniteMassCorrection()
    {
        for (auto& manifold : manifolds)
            if (manifold.contactCount)
                manifold.infiniteMassCorrection();
    }
}

