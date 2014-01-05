#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include <set>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <Fission/Core/Component.h>

#include "phys/RigidBody.h"

class Entity;
class PhysicsSystem;
class GridComponent;

const float PTU = 16.f;

class PhysicsComponent : public fsn::Component
{
    FISSION_COMPONENT

    friend class PhysicsSystem;

    public:
        PhysicsComponent(int width = 16, int height = 16, float density = 1.f);
        PhysicsComponent(float radius, float density);
        PhysicsComponent(sf::Vector2f* verts, int vertCount, float density = 1.f);
        virtual ~PhysicsComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

        // Getters
        phys::RigidBody* getBody(){return mBody;}

    private:
        phys::RigidBody* mBody;

        float mDensity;
};

#endif // PHYSICSCOMPONENT_H
