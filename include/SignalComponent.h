#ifndef SIGNALCOMPONENT_H
#define SIGNALCOMPONENT_H

#include <SFML/System/Vector2.hpp>

#include <Fission/Core/Component.h>

class SignalComponent : public Component
{
    public:
        enum SignalType
        {
            None,
            Integer,
            Float,
            Vector
        };

        SignalComponent();
        virtual ~SignalComponent();

        void addOutput(SignalComponent* output){outputs.push_back(output);}

        void fireInt(int i);
        void fireFloat(float f);
        void fireVector(const sf::Vector2f& v);

        bool hasSignal(){return type!=SignalComponent::None;}
        int getInt();
        float getFloat();
        sf::Vector2f getVector();

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new SignalComponent;}

    private:
        void clearData();

        void receiveSignal(int i);
        void receiveSignal(float f);
        void receiveSignal(const sf::Vector2f& v);

        std::vector<SignalComponent*> outputs;

        // Received data
        SignalType type;
        void* data;
};

#endif // SIGNALCOMPONENT_H
