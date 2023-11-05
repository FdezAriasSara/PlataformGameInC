#pragma once

#include "Layer.h"
#include "Player.h"
#include "Background.h"

#include "Enemy.h"
#include "Projectile.h"
#include "Text.h"
#include "Tile.h"
#include "Pad.h"
#include "Collectable.h"
#include "CheckPoint.h"
#include "BrittleTile.h"
#include "Audio.h"
#include "Space.h" // importar

#include <fstream> // Leer ficheros
#include <sstream> // Leer líneas / String
#include <list>

class GameLayer : public Layer
{
public:
	GameLayer(Game* game);
	void init() override;
	void processControls() override;
	void update() override;
	void draw() override;
	void keysToControls(SDL_Event event);
	void mouseToControls(SDL_Event event); // USO DE MOUSE
	void gamePadToControls(SDL_Event event); // USO DE GAMEPAD
	void loadMap(string name);
	void loadMapObject(char character, float x, float y);
	void calculateScroll();
	Actor* message;
	bool pause;
	// Elementos de interfaz
	SDL_GameController* gamePad;
	Pad* pad;
	Actor* buttonJump;
	Actor* buttonShoot;

	Tile* cup; // Elemento de final de nivel
	Space* space;
	float scrollX;
	float scrollY;//Ampliación - Scrolly
	int mapWidth;
	list<Tile*> tiles;
	int xCheckpoint;//Ampliación-puntos de guardado
	int yCheckpoint;//Ampliación-puntos de guardado
	Audio* audioBackground;
	Text* textPoints;
	int points;
	Text* textCollectables;//Ampliación - Recolectables
	int collectableCount;//Ampliación - Recolectables
	int newEnemyTime = 0;
	Player* player;
	list<Collectable*> collectables;
	Background* background;
	Actor* backgroundPoints;
	Actor* backgroundCollectables;//Ampliación - Recolectables
	list<CheckPoint*> checkpoints;//Ampliación-puntos de guardado
	list<BrittleTile*> brittleTiles;//Ampliación- Tiles frágiles ( destruidas al estar encima)
	list<Enemy*> enemies;
	list<Projectile*> projectiles;

	bool controlContinue = false;
	bool controlShoot = false;
	int controlMoveY = 0;
	int controlMoveX = 0;


};

