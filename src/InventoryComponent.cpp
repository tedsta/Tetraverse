#include "InventoryComponent.h"

#include "ItemComponent.h"

TypeBits InventoryComponent::Type;

InventoryComponent::InventoryComponent(int size) : mSlots(size)
{
    for (unsigned int i = 0; i < mSlots.size(); i++)
    {
        mSlots[i].mItem = -1;
        mSlots[i].mStack = 0;
    }
}

InventoryComponent::~InventoryComponent()
{
    //dtor
}

void InventoryComponent::serialize(sf::Packet &packet)
{
    packet << static_cast<int>(mSlots.size());
    for (auto& slot : mSlots)
        packet << slot.mItem << slot.mStack;
}

void InventoryComponent::deserialize(sf::Packet &packet)
{
    int slotCount;
    packet >> slotCount;
    mSlots.resize(slotCount);
    for (int i = 0; i < slotCount; i++)
        packet >> mSlots[i].mItem >> mSlots[i].mStack;
}

void InventoryComponent::postDeserialize()
{
}

int InventoryComponent::addItem(int slot, int item, int count)
{
    if (slot >= mSlots.size() || (mSlots[slot].mItem != -1 && mSlots[slot].mItem != item) ||
        mSlots[slot].mStack >= Item::Items[item]->getMaxStack() || item >= Item::Items.size())
        return count;

    int transfer = std::min(count, Item::Items[item]->getMaxStack() - mSlots[slot].mStack);
    mSlots[slot].mItem = item;
    mSlots[slot].mStack += transfer;
    return count-transfer;
}

int InventoryComponent::addItem(int item, int count)
{
    if (item >= Item::Items.size())
        return count;

    // See if there's a stack or two we can put these items on
    for (unsigned int i = 0; i < mSlots.size(); i++)
    {
        if (mSlots[i].mItem == item && mSlots[i].mStack < Item::Items[item]->getMaxStack())
        {
            count = addItem(i, item, count);
            if (count == 0)
                return 0;
        }
    }

    // Put the leftovers somewhere
    for (unsigned int i = 0; i < mSlots.size(); i++)
    {
        if (mSlots[i].mItem == -1)
        {
            count = addItem(i, item, count);
            if (count == 0)
                return 0;
        }
    }

    return count;
}

bool InventoryComponent::useItem(int slot, Entity* grid, std::queue<sf::Vector2f>& coords)
{
    if (mSlots[slot].mItem == -1)
        return false;

    int item = mSlots[slot].mItem;

    bool used = Item::Items[item]->use(grid, coords);

    if (used && Item::Items[item]->getConsumable())
    {
        mSlots[slot].mStack--;
        if (mSlots[slot].mStack == 0)
            mSlots[slot].mItem = -1;
    }

    return used;
}

Item* InventoryComponent::getItemAt(int slot)
{
    if (mSlots[slot].mItem == -1)
        return NULL;
    else
        return Item::Items[mSlots[slot].mItem];
}
