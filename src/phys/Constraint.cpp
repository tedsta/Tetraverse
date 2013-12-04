#include "phys/Constraint.h"

#include "phys/RigidBody.h"

namespace phys
{
    Constraint::Constraint(RigidBody* _a, RigidBody* _b) : a(_a), b(_b)
    {
        //ctor
    }

    Constraint::~Constraint()
    {
        //dtor
    }

    void Constraint::applyImpulse(const sf::Vector2f& i)
    {
        a->velocity += i * a->inverseMass;
		b->velocity -= i * b->inverseMass;
    }

    // ********************************************************************************************

    void DistanceConstraint::solve(const float dt)
    {
        // get some information that we need
		sf::Vector2f axis = getB()->getPosition() - getA()->getPosition();
		float currentDistance = length(axis);
		sf::Vector2f unitAxis = axis * (1.f/currentDistance);

		// calculate relative velocity in the axis, we want to remove this
		float relVel = dot(getB()->getVelocity() - getA()->getVelocity(), unitAxis);

		float relDist = currentDistance-distance;

		// calculate impulse to solve
		float remove = relVel+relDist/dt;
		float impulse = remove / (getA()->getInverseMass() + getB()->getInverseMass());

		// generate impulse vector
		sf::Vector2f I = unitAxis*impulse;

		// apply
		applyImpulse(I);
    }
}


