#include "SignalComponent.h"

TypeBits SignalComponent::Type;

SignalComponent::SignalComponent() : type(SignalComponent::None), data(NULL)
{
    //ctor
}

SignalComponent::~SignalComponent()
{
    //dtor
}

void SignalComponent::serialize(sf::Packet &packet)
{
}

void SignalComponent::deserialize(sf::Packet &packet)
{
}

void SignalComponent::fireInt(int i)
{
    for (auto output : outputs)
        output->receiveSignal(i);
}

void SignalComponent::fireFloat(float f)
{
    for (auto output : outputs)
        output->receiveSignal(f);
}

void SignalComponent::fireVector(const sf::Vector2f& v)
{
    for (auto output : outputs)
        output->receiveSignal(v);
}

void SignalComponent::clearData()
{
    switch (type)
    {
    case SignalComponent::Integer:
        delete reinterpret_cast<int*>(data);
        break;
    case SignalComponent::Float:
        delete reinterpret_cast<float*>(data);
        break;
    case SignalComponent::Vector:
        delete reinterpret_cast<sf::Vector2f*>(data);
        break;
    }

    type = SignalComponent::None;
}

void SignalComponent::receiveSignal(int i)
{
    clearData();
    type = SignalComponent::Integer;
    data = new int(i);
}

void SignalComponent::receiveSignal(float f)
{
    clearData();
    type = SignalComponent::Float;
    data = new float(f);
}

void SignalComponent::receiveSignal(const sf::Vector2f& v)
{
    clearData();
    type = SignalComponent::Vector;
    data = new sf::Vector2f(v);
}

int SignalComponent::getInt()
{
    if (type == SignalComponent::Integer)
    {
        void* oData = data;
        clearData();
        return *reinterpret_cast<int*>(oData);
    }
    else
        return 0;
}

float SignalComponent::getFloat()
{
    if (type == SignalComponent::Integer)
    {
        void* oData = data;
        clearData();
        return *reinterpret_cast<float*>(oData);
    }
    else
        return 0;
}

sf::Vector2f SignalComponent::getVector()
{
    if (type == SignalComponent::Integer)
    {
        void* oData = data;
        clearData();
        return *reinterpret_cast<sf::Vector2f*>(oData);
    }
    else
        return sf::Vector2f(0, 0);
}

