#pragma once
#include "Actor.h"
class FragileTile :
    public Actor
{
//Ampliaci�n-Tiles destructibles con disparo.
public:
    FragileTile(string filename, int shots, float x, float y, Game* game);
    int shotsLeft;
    
};

