#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <Fission/Core/Component.h>

#include "phys/RigidBody.h"

class Entity;
class PhysicsSystem;

class PhysicsComponent : public Component
{
    friend class PhysicsSystem;

    public:
        PhysicsComponent(PhysicsSystem* physSys = NULL, int width = 16, int height = 16);
        virtual ~PhysicsComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

        // Setters
        void setGrid(Entity* g){grid=g;}

        // Getters
        phys::RigidBody* getBody(){return body;}
        Entity* getGrid(){return grid;}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new PhysicsComponent;}

    private:
        phys::RigidBody* body;
        Entity* grid;
};

#endif // PHYSICSCOMPONENT_H
