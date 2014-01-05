#ifndef TETRA_RIGIDBODY_H
#define TETRA_RIGIDBODY_H

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

class RigidBody : public fsn::Component
{
    FISSION_COMPONENT

    friend class PhysicsSystem;

    public:
        RigidBody(int width = 16, int height = 16, float density = 1.f);
        RigidBody(float radius, float density);
        RigidBody(sf::Vector2f* verts, int vertCount, float density = 1.f);
        virtual ~RigidBody();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

        // Getters
        phys::RigidBody* getBody(){return mBody;}

    private:
        phys::RigidBody* mBody;

        float mDensity;
};

#endif // TETRA_RIGIDBODY_H
