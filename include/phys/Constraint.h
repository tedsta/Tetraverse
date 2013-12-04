#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <SFML/System/Vector2.hpp>

namespace phys
{
    class RigidBody;

    class Constraint
    {
        public:
            Constraint(RigidBody* _a, RigidBody* _b);
            virtual ~Constraint();

            void applyImpulse(const sf::Vector2f& i);
            virtual void solve(const float dt) = 0;

            RigidBody* getA(){return a;}
            RigidBody* getB(){return b;}

        private:
            RigidBody* a;
            RigidBody* b;
    };

    class DistanceConstraint : public Constraint
    {
        public:
            DistanceConstraint(RigidBody* a, RigidBody* b, float dist) : Constraint(a, b), distance(dist) {}

            void solve(const float dt);

        private:
            float distance;
    };
}


#endif // CONSTRAINT_H
