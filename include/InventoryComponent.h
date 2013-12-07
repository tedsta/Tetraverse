#ifndef INVENTORYCOMPONENT_H
#define INVENTORYCOMPONENT_H

#include <vector>
#include <queue>
#include <SFML/System/Vector2.hpp>
#include <Fission/Core/Component.h>

class Item;
class Entity;

class InventoryComponent : public Component
{
    public:
        InventoryComponent(int size);
        virtual ~InventoryComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);

        int addItem(int slot, int item, int count);
        int addItem(int item, int count);

        bool useItem(int slot, Entity* grid, std::queue<sf::Vector2f>& coords);

        Item* getItemAt(int slot);

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new InventoryComponent(0);}

    private:
        struct Slot
        {
            int mItem; // Item ID
            int mStack; // Stack count
        };

        std::vector<Slot> mSlots;
};

#endif // INVENTORYCOMPONENT_H
