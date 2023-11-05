#pragma once
#include "Actor.h"
class BrittleTile :
    public Actor
{   public:
        BrittleTile(string filename,int seconds, float x, float y, Game* game);
        int secondsLeft;//Ampliación-Tiles destructibles al estar encima
        bool isOverlap(Actor* actor);
};

