#ifndef WEAPONCOMPONENT_H
#define WEAPONCOMPONENT_H

#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <Fission/Core/Component.h>

class Entity;

class WeaponComponent : public Component
{
    public:
        WeaponComponent();
        virtual ~WeaponComponent();

        // Serialization stuff
        void serialize(sf::Packet &packet);
        void deserialize(sf::Packet &packet);
        void postDeserialize();

        bool fire(float rot);

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new WeaponComponent;}

    private:
        int mDamage;
        float mRange;
        sf::Vector2f mStartPosition;
        float mDelay;
        sf::Clock mDelayClock;

        Entity* mProjectile;
};

#endif // WEAPONCOMPONENT_H
