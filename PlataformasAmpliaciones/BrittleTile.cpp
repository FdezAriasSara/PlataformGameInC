#include  "BrittleTile.h"

BrittleTile::BrittleTile(string filename, int seconds,float x, float y, Game* game)
	: Actor(filename, x, y, 40, 32, game) 
		{
			this->secondsLeft = seconds;



		}
 
bool BrittleTile::isOverlap(Actor* actor) {
	bool overlap = false;
	if (actor->x   <= x + width / 2
		&& actor->x  >= x - width / 2
		&& actor->y + actor->height / 2 == y - height/2 //Solo si está encima
		 
		) {
 		overlap = true;
	}
	return overlap;

}
;
	