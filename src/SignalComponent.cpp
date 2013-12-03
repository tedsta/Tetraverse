#include "SignalComponent.h"

TypeBits SignalComponent::Type;

SignalComponent::SignalComponent()
{
    //ctor
}

SignalComponent::~SignalComponent()
{
    //dtor
}

void SignalComponent::fireSignal(int i)
{
    for (auto output : outputs)
    {

    }
}
