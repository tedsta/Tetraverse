#ifndef SIGNALCOMPONENT_H
#define SIGNALCOMPONENT_H

#include <Fission/Core/Component.h>

class SignalComponent : public Component
{
    public:
        enum SignalType
        {
            Integer,
            Real,
            Vector
        };

        SignalComponent();
        virtual ~SignalComponent();

        void fireSignal(int i);

        static TypeBits Type;
        const TypeBits getTypeBits() const {return Type;}
        static Component* factory() {return new SignalComponent;}

    private:
        std::vector<SignalComponent*> outputs;
};

#endif // SIGNALCOMPONENT_H
