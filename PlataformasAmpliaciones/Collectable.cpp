#include "Collectable.h"

  Collectable :: Collectable(float x, float y, Game* game) : Actor("res/recolectable.png", x, y,32, 32,game){

	  animation =new Animation("res/recolectable.png", width, height,
		  256, 32, 6, 8, true, game);
	  audio =  Audio::createAudio("res/Rise07.wav", false);

};
  void Collectable::update() {
	  animation->update();
  } 
  void Collectable::draw(float scrollX, float scrollY) {
	  animation->draw(x-scrollX, y-scrollY);
  }