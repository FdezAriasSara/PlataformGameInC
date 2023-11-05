#include "FragileTile.h"
FragileTile::FragileTile(string filename, int shotsLeft, float x, float y, Game* game)
	: Actor(filename, x, y, 40, 32, game)
{
	this->shotsLeft = shotsLeft;



}