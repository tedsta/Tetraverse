#include "WireComponent.h"

#include <cmath>
#include <iostream>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

int randStateCovered();
int randStateTop();
int randStateBot();
int randStateRight();
int randStateLeft();

TypeBits WireComponent::Type;

WireComponent::WireComponent()
{
        red = NULL;
        black = NULL;
        power = 0;
        drain = 0;
        ground = 0;
}

void WireComponent::update(){
    int total = wire->ground+ground;
    if(wire = NULL){
        if(power - ground >= drain>>1){
            power -= drain>>1;
            ground += drain>>1;
            drain = 0;
        }
        return;
    }
    ground = (total > WireMax?WireMax:total);
    wire->ground=total>WireMax?total-WireMax:0;
    if(power - ground >= drain>>1){
        power -= drain>>1;
        ground += drain>>1;
        drain = 0;
    }
    total = wire->power+power;
    wire->power = total > WireMax?WireMax:total;
    power = total > WireMax ? total-WireMax:0;
    return wire->update();
}

int WireComponent::load(int load){
    int value = power - ground >= drain + load ? load : power - ground - drain;
    drain += value;
    return value;
}
