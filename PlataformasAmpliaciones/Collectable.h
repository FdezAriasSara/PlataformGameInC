#pragma once
#include "Actor.h"
#include "Animation.h"
#include "Audio.h"
class Collectable:public Actor
{
public :
		Collectable(float x, float y, Game* game);	
		Animation* animation; 
		Audio* audio;
		void update(); 
		void draw(float scrollX = 0, float scrollY = 0) override;
};

