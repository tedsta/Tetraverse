#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <set>
#include <SFML/System/Vector2.hpp>

#include "PhysMath.h"

namespace phys
{
    class Shape;

    class RigidBody
    {
        friend class Manifold;
        friend class Collision;
        friend class CircleShape;
        friend class PolygonShape;
        friend class PhysicsWorld;
        friend class Constraint;

        public:
            RigidBody(Shape* _shape, int _type, float density);
            virtual ~RigidBody();

            void applyForce(const sf::Vector2f& f){force+=f;}
            void applyImpulse(const sf::Vector2f& impulse, const sf::Vector2f& contactVector)
            {
                velocity += inverseMass * impulse;
                angularVelocity += inverseInertia * cross(contactVector, impulse);
            }
            void addVelocity(const sf::Vector2f& vel){velocity+=vel;}
            void addAngularVelocity(float vel){angularVelocity+=vel;}

            void setStatic()
            {
                mass = inverseMass = mass_ = inverseMass_ = 0.f;
                inertia = inverseInertia = inertia_ = inverseInertia_ = 0.f;
            }

            void setFixedRotation()
            {
                inertia = inverseInertia = inertia_ = inverseInertia_ = 0.f;
            }

            // Setters
            void setType(int t){type=t;}
            void setGravity(const sf::Vector2f& g){gravity=g;}
            void setPosition(const sf::Vector2f& pos){position=pos;}
            void setVelocity(const sf::Vector2f& vel){velocity=vel;}
            void setVelocityX(int x){velocity.x=x;}
            void setVelocityY(int y){velocity.y=y;}
            void setRotation(float rot);
            void setAngularVelocity(float vel){angularVelocity=vel;}
            void setInertia(float i){inertia=i;}
            void setInverseInertia(float ii){inverseInertia=ii;}
            void setMass(float m){mass=m;}
            void setInverseMass(float m){inverseMass=m;}
            void setTorque(float t){torque=t;}
            void setStaticFriction(float friction){staticFriction=friction;}
            void setDynamicFriction(float friction){dynamicFriction=friction;}
            void setRestitution(float res){restitution=res;}
            void setShape(Shape* s){shape=s;}
            void setParent(RigidBody* body)
            {
                if (parent)
                    parent->children.erase(this);

                parent=body;
                if (parent)
                    parent->children.insert(this);
            }

            // Getters
            int getType(){return type;}
            sf::Vector2f getGravity(){return gravity;}
            sf::Vector2f getPosition(){return position;}
            sf::Vector2f getVelocity(){return velocity;}
            float getRotation(){return rotation;}
            float getAngularVelocity(){return angularVelocity;}
            float getTorque(){return torque;}
            float getInertia(){return inertia;}
            float getInverseInertia(){return inverseInertia;}
            float getMass(){return mass;}
            float getInverseMass(){return inverseMass;}
            float getStaticFriction(){return staticFriction;}
            float getDynamicFriction(){return dynamicFriction;}
            float getRestitution(){return restitution;}
            Shape* getShape(){return shape;}

        private:
            int type;

            sf::Vector2f gravity;

            sf::Vector2f position;
            sf::Vector2f velocity;

            sf::Vector2f oldPosition;
            sf::Vector2f velocity_;
            float angularVelocity_;

            float rotation;
            float angularVelocity;
            float torque;

            sf::Vector2f force;

            float inertia;  // moment of inertia
            float inverseInertia; // inverse inertia
            float mass;  // mass
            float inverseMass; // inverse mass

            float inertia_;  // moment of inertia
            float inverseInertia_; // inverse inertia
            float mass_;  // mass
            float inverseMass_; // inverse mass

            float staticFriction;
            float dynamicFriction;
            float restitution;

            Shape* shape;

            RigidBody* parent;
            std::set<RigidBody*> children;
    };
}

#endif // RIGIDBODY_H
