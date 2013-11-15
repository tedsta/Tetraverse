#ifndef INVENTORYCOMPONENT_H
#define INVENTORYCOMPONENT_H

#include <vector>
#include <Fission/Core/Component.h>

class Item;
class GridComponent;

class InventoryComponent : public Component
{
    public:
        InventoryComponent(int size);
        virtual ~InventoryComponent();

        int addItem(int slot, int item, int count);
        int addItem(int item, int count);

        bool useItem(int slot, GridComponent* grid, int mouseX, int mouseY);

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
