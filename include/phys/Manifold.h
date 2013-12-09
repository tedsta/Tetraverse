#ifndef MANIFOLD_H
#define MANIFOLD_H

#include <vector>

#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>

namespace phys
{
    class RigidBody;

    class Manifold
    {
        public:
            Manifold(RigidBody* a, RigidBody* b, float _dt, const sf::Vector2f& _gravity);
            virtual ~Manifold();

            void initialize();            // Precalculations for impulse solving
            void applyImpulse();          // Solve impulse and apply
            void positionalCorrection();  // Naive correction of positional penetration
            void infiniteMassCorrection();

            RigidBody* bodyA;
            RigidBody* bodyB;

            float penetration;        // Depth of penetration from collision
            sf::Vector2f normal;      // From A to B
            sf::Vector2f contacts[2]; // Points of contact during collision
            sf::Uint32 contactCount;  // Number of contacts that occured during collision
            float restitution;        // Mixed restitution
            float dynamicFriction;    // Mixed dynamic friction
            float staticFriction;     // Mixed static friction

        private:
            float dt;
            sf::Vector2f gravity;
    };

    class Collision
    {
        public:
            Collision(RigidBody* a, RigidBody* b, float _dt, const sf::Vector2f& _gravity);

            void addManifold();
            Manifold* getLastManifold(){return &manifolds.back();}
            bool hasCollision() const {return collision;}

            void solve(); // Generate contact information
            void initialize();            // Precalculations for impulse solving
            void applyImpulse();          // Solve impulse and apply
            void positionalCorrection();  // Naive correction of positional penetration
            void infiniteMassCorrection();

        private:
            RigidBody* bodyA;
            RigidBody* bodyB;
            float dt;
            sf::Vector2f gravity;
            std::vector<Manifold> manifolds;
            bool collision;
    };
}


#endif // MANIFOLD_H
