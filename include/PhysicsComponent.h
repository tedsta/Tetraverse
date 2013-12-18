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

class PhysicsComponent : public Component
{
    friend class PhysicsSystem;

    public:
        PhysicsComponent(int width = 16, int height = 16, float _density = 1.f);
        PhysicsComponent(float radius, float _density);
        PhysicsComponent(sf::Vector2f* verts, int vertCount, float _density = 1.f);
        PhysicsComponent(GridComponent* gridCmp);
        virtual ~PhysicsComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);
        void postDeserialize();

        void addGrid(Entity* g);
        void removeGrid(Entity* g);

        // Getters
        phys::RigidBody* getBody(){return body;}
        Entity* getPrimaryGrid(){return primaryGrid;}

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new PhysicsComponent;}

    private:
        void recalculatePrimaryGrid();

        phys::RigidBody* body;
        Entity* primaryGrid;

        float density;

        std::set<Entity*> grids;
};

#endif // PHYSICSCOMPONENT_H
