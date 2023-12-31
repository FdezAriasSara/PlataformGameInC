#include "GameLayer.h"

GameLayer::GameLayer(Game* game)
	: Layer(game) {
	//llama al constructor del padre : Layer(renderer)
	pause = true;
	message = new Actor("res/mensaje_como_jugar.png", WIDTH * 0.5, HEIGHT * 0.5,
		WIDTH, HEIGHT, game);

	gamePad = SDL_GameControllerOpen(0);
	init();
}


void GameLayer::init() {
	pad = new Pad(WIDTH * 0.15, HEIGHT * 0.80, game);

	buttonJump = new Actor("res/boton_salto.png", WIDTH * 0.9, HEIGHT * 0.55, 100, 100, game);
	buttonShoot = new Actor("res/boton_disparo.png", WIDTH * 0.75, HEIGHT * 0.83, 100, 100, game);

	space = new Space(1);
	scrollX = 0;
	scrollY = 0;//Ampliación- Scrolly
	tiles.clear();

	audioBackground = Audio::createAudio("res/musica_ambiente.mp3", true);
	audioBackground->play();

	points = 0;
	textPoints = new Text("0", WIDTH * 0.92, HEIGHT * 0.04, game);
	textPoints->content = to_string(points);


	
	//Ampliación - Recolectables
	collectableCount = 0;
	textCollectables = new Text("0", WIDTH * 0.77, HEIGHT * 0.04, game);
	textCollectables->content = to_string(collectableCount);


	


	background = new Background("res/fondo_2.png", WIDTH * 0.5, HEIGHT * 0.5, -1, game);
	backgroundPoints = new Actor("res/icono_puntos.png",
		WIDTH * 0.85, HEIGHT * 0.05, 24, 24, game);
	backgroundCollectables = new Actor("res/icono_recolectable.png",
		WIDTH * 0.69, HEIGHT * 0.07,40 , 40, game);

	enemies.clear(); // Vaciar por si reiniciamos el juego
	projectiles.clear(); // Vaciar por si reiniciamos el juego
	collectables.clear();//Ampliación-Recolectables.
	checkpoints.clear();//Ampliación checkpoints
	brittleTiles.clear();//Ampliación - Tiles frágiles al estar encima
	fragileTiles.clear();//Ampliación - Tiles destructibles al disparar
	loadMap("res/" + to_string(game->currentLevel) + ".txt");
}

void GameLayer::loadMap(string name) {
	char character;
	string line;
	ifstream streamFile(name.c_str());
	if (!streamFile.is_open()) {
		cout << "Falla abrir el fichero de mapa" << endl;
		return;
	}
	else {
		// Por línea
		for (int i = 0; getline(streamFile, line); i++) {
			istringstream streamLine(line);
			mapWidth = line.length() * 40; // Ancho del mapa en pixels
			// Por carácter (en cada línea)
			for (int j = 0; !streamLine.eof(); j++) {
				streamLine >> character; // Leer character 
				cout << character;
				float x = 40 / 2 + j * 40; // x central
				float y = 32 + i * 32; // y suelo
				loadMapObject(character, x, y);
			}

			cout << character << endl;
		}
	}
	streamFile.close();
}

void GameLayer::loadMapObject(char character, float x, float y)
{
	switch (character) {
	case 'C': {
		cup = new Tile("res/copa.png", x, y, game);
		// modificación para empezar a contar desde el suelo.
		cup->y = cup->y - cup->height / 2;
		space->addDynamicActor(cup); // Realmente no hace falta
		break;
	}
	case 'E': {
		Enemy* enemy = new Enemy(x, y, game);
		// modificación para empezar a contar desde el suelo.
		enemy->y = enemy->y - enemy->height / 2;
		enemies.push_back(enemy);
		space->addDynamicActor(enemy);
		break;
	}
	case '1': {
		if (xCheckpoint && yCheckpoint) {//Ampliación - Puntos de guardado
			player = new Player(xCheckpoint, yCheckpoint, game);
		}
		else {
			player = new Player(x, y, game);
		}		
		// modificación para empezar a contar desde el suelo.
		player->y = player->y - player->height / 2;
		space->addDynamicActor(player);
		break;
	}
	case '#': {
		Tile* tile = new Tile("res/bloque_tierra.png", x, y, game);
		// modificación para empezar a contar desde el suelo.
		tile->y = tile->y - tile->height / 2;
		tiles.push_back(tile);
		space->addStaticActor(tile);
		break;
	}
	//Ampliación recolectables
	case 'P': {
		Collectable* collectable = new Collectable(x,y,game);
		collectables.push_back(collectable);
		collectable->y = collectable->y - collectable->height / 2;
		space->addDynamicActor(collectable);  
		break;
	}
	//Ampliación - Punto de guardado
	case 'A': {
		CheckPoint* checkpoint = new CheckPoint("res/floppyDisk.png", x, y, game);
		checkpoints.push_back(checkpoint);
		checkpoint->y = checkpoint->y - checkpoint->height / 2;
		space->addDynamicActor(checkpoint);
		break;
	}
	//Ampliación - Tiles frágiles al estar encima
	case 'W': {
		BrittleTile* tile = new BrittleTile("res/bloque_fondo_muro.png",10, x, y, game);
		brittleTiles.push_back(tile);
		tile->y = tile->y - tile->height / 2;
		space->addStaticActor(tile);
		break;
	}
	//Ampliación - Tiles destructibles mediante disparos
	case 'U': {
		FragileTile* tile = new FragileTile("res/bloque_metal.png", 2, x, y, game);
		fragileTiles.push_back(tile);
		tile->y = tile->y - tile->height / 2;
		space->addStaticActor(tile);
		break;
	}
	}
}


void GameLayer::processControls() {
	// obtener controles
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_CONTROLLERDEVICEADDED) {
			gamePad = SDL_GameControllerOpen(0);
			if (gamePad == NULL) {
				cout << "error en GamePad" << endl;
			}
			else {
				cout << "GamePad conectado" << endl;
			}
		}

		// Cambio automático de input
		// PONER el GamePad
		if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERAXISMOTION) {
			game->input = game->inputGamePad;
		}
		if (event.type == SDL_KEYDOWN) {
			game->input = game->inputKeyboard;
		}
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			game->input = game->inputMouse;
		}
		// Procesar teclas
		if (game->input == game->inputKeyboard) {
			keysToControls(event);
		}
		if (game->input == game->inputMouse) {
			mouseToControls(event);
		}
		// Procesar Mando
		if (game->input == game->inputGamePad) {  // gamePAD
			gamePadToControls(event);
		}


	}
	//procesar controles
	//procesar controles
	// Disparar
	if (controlContinue) {
		pause = false;
		controlContinue = false;
	}
	if (controlShoot) {
		Projectile* newProjectile = player->shoot();
		if (newProjectile != NULL) {
			space->addDynamicActor(newProjectile);
			projectiles.push_back(newProjectile);
		}

	}

	// Eje X
	if (controlMoveX > 0) {
		player->moveX(1);
	}
	else if (controlMoveX < 0) {
		player->moveX(-1);
	}
	else {
		player->moveX(0);
	}

	// Eje Y
	if (controlMoveY > 0) {
	
	}
	else if (controlMoveY < 0) {
		player->jump();
	}
	else {

	}



}

void GameLayer::update() {
	if (pause) {
		return;
	}

	// Nivel superado
	if (cup->isOverlap(player)) {
		game->currentLevel++;
		if (game->currentLevel > game->finalLevel) {
			game->currentLevel = 0;
		}
		message = new Actor("res/mensaje_ganar.png", WIDTH * 0.5, HEIGHT * 0.5,
			WIDTH, HEIGHT, game);
		pause = true;
		init();
	}

	// Jugador se cae
	if (player->y > HEIGHT + 80) {
		init();
	
	}

	space->update();
	background->update();
	player->update();
	for (auto const& enemy : enemies) {
		enemy->update();
	}
	for (auto const& projectile : projectiles) {
		projectile->update();
	}
	//Ampliación - Recolectables
	for (auto const& collectable : collectables) {
		collectable->update();
	}
	
	// Colisiones
	for (auto const& enemy : enemies) {
		if (player->isOverlap(enemy)) {
			player->loseLife();
			if (player->lifes <= 0) {
				init();
				return;
			}
		}
	}
	//Ampliaciones - Punto de guardado 
	list<CheckPoint*> deleteCheckp;
	for (auto const& checkp :checkpoints) {
		bool cInList = std::find(deleteCheckp.begin(),
			deleteCheckp.end(),
			checkp) != deleteCheckp.end();
		if (player->isOverlap(checkp)) {
			if (!cInList) { 
				xCheckpoint= checkp->x;
				yCheckpoint = checkp->y;
				deleteCheckp.push_back(checkp);
				break;

			}
		}
	}
	// Colisiones recolectable y player.
	list<Collectable*> deleteCollectables;
	for (auto const& collectable : collectables) {
		bool cInList = std::find(deleteCollectables.begin(),
			deleteCollectables.end(),
			collectable) != deleteCollectables.end();
		if (player->isOverlap(collectable)) {
			if (!cInList) {
				collectable->audio->play();//Reproducir sonido
				collectableCount++;
				textCollectables->content = to_string(collectableCount);
				deleteCollectables.push_back(collectable);


			}
		}
	}
	//Ampliación - Tiles frágiles al estar encima
	list<BrittleTile*> deleteBtiles;
	for (auto const& btile : brittleTiles) {
		 
			bool bInList = std::find(deleteBtiles.begin(),
				deleteBtiles.end(),
				btile) != deleteBtiles.end();
			if (btile->isOverlap(player)) {
				if (!bInList && btile->secondsLeft==0  ) {
					deleteBtiles.push_back(btile);
				}
				else {
					btile->secondsLeft--;
				}
			 
			}
		}
	

 
	// Colisiones , Enemy - Projectile


	list<Projectile*> deleteProjectiles;
	for (auto const& projectile : projectiles) {
		if (projectile->isInRender(scrollX,scrollY) == false || projectile->vx == 0) {

			bool pInList = std::find(deleteProjectiles.begin(),
				deleteProjectiles.end(),
				projectile) != deleteProjectiles.end();

			if (!pInList) {
				deleteProjectiles.push_back(projectile);
			}
		}
	}
	//Ampliación - Tiles frágiles al disparo
	list<FragileTile*> deleteFtiles;
	for (auto const& fTile : fragileTiles) {
		for (auto const& projectile : projectiles) {
			bool fInList = std::find(deleteFtiles.begin(),
				deleteFtiles.end(),
				fTile) != deleteFtiles.end();
			 

			if (projectile->isOverlap(fTile)) {
				if (!fInList && fTile->shotsLeft == 0) {
					deleteFtiles.push_back(fTile);
				}
				else {
					fTile->shotsLeft--;
				}
			}

		}
	}

	for (auto const& enemy : enemies) {
		for (auto const& projectile : projectiles) {
			if (enemy->isOverlap(projectile)) {
				bool pInList = std::find(deleteProjectiles.begin(),
					deleteProjectiles.end(),
					projectile) != deleteProjectiles.end();

				if (!pInList) {
					deleteProjectiles.push_back(projectile);
				}


				enemy->impacted();
				points++;
				textPoints->content = to_string(points);


			}
		}
	}

	list<Enemy*> deleteEnemies;
	for (auto const& enemy : enemies) {
		if (enemy->state == game->stateDead) {
			bool eInList = std::find(deleteEnemies.begin(),
				deleteEnemies.end(),
				enemy) != deleteEnemies.end();

			if (!eInList) {
				deleteEnemies.push_back(enemy);
			}
		}
	}
	//Ampliación - Tiles frágiles al peso 
	for (auto const& deltile : deleteBtiles) {
	 
		brittleTiles.remove(deltile);
		space->removeStaticActor(deltile);
	}
	deleteBtiles.clear();
	//Ampliación - Tiles frágiles al disparo
	for (auto const& fTile : deleteFtiles) {
		fragileTiles.remove(fTile);
		space->removeStaticActor(fTile);
	}

	deleteFtiles.clear();
	//Ampliación-Recolectables
	for (auto const& delCollect : deleteCollectables) {
		collectables.remove(delCollect);
		space->removeStaticActor(delCollect);
	}
	//Ampliación - Puntos de guardado
	deleteCollectables.clear();
	for (auto const& delCheckp : deleteCheckp) {
		checkpoints.remove(delCheckp);
		space->removeStaticActor(delCheckp);
	
	}
	deleteCheckp.clear();

	for (auto const& delEnemy : deleteEnemies) {
		enemies.remove(delEnemy);
		space->removeDynamicActor(delEnemy);
	}
	deleteEnemies.clear();

	for (auto const& delProjectile : deleteProjectiles) {
		projectiles.remove(delProjectile);
		space->removeDynamicActor(delProjectile);
		delete delProjectile;
	}
	deleteProjectiles.clear();



}

void GameLayer::calculateScroll() {
 
	// limite superior - Ampliación ScrollY	
	if (player->y > HEIGHT *0.1) { //cuando la altura del player es mayor que el 10% de la altura de la pantalla
		if (player->y- scrollY < HEIGHT*0.3) {	//Cuando la altura es menor que el 30% habiendo aplicado el scroll actual		
			scrollY = player->y -HEIGHT * 0.3; //el scroll será la diferencia entre la altura y el 30%
		}
    }

	// limite inferior - Ampliación ScrollY	
	if (player->y <  HEIGHT * 0.7) { //Cuando el jugador se encuentra por debajo del 70% de la altura de la pantalla
		if (player->y - scrollY > HEIGHT * 0.7) {//Si el scroll actual   es superior a dicho 70% , significa que esta "muy alto"
			scrollY = player->y - HEIGHT * 0.7;//Por tanto readaptamos el scroll de altura del jugador para que haga que se posicione por debajo de dicho 70%
		}
	}

	
	// limite izquierda
	if (player->x > WIDTH * 0.3) {
		if (player->x - scrollX < WIDTH * 0.3) {
			scrollX = player->x - WIDTH * 0.3;
		}
	}

	// limite derecha
	if (player->x < mapWidth - WIDTH * 0.3) {
		if (player->x - scrollX > WIDTH * 0.7) {
			scrollX = player->x - WIDTH * 0.7;
		}
	}

	

	
}


void GameLayer::draw() { 
	calculateScroll();

	background->draw();
	for (auto const& tile : tiles) {
		tile->draw(scrollX,scrollY);
	}
	//Ampliación- Tiles frágiles
	for (auto const& btile : brittleTiles) {
		btile->draw(scrollX, scrollY);
	}
	//Ampliación- Tiles frágiles al disparo
	for (auto const& ftile : fragileTiles) {
		ftile->draw(scrollX, scrollY);
	}
	//Ampliación- Recolectables 
	for (auto const& collectable : collectables) {
		collectable->draw(scrollX, scrollY);// Ampliación- Scrollx
	}
	//Ampliación- Puntos de guardado
	for (auto const& checkp : checkpoints) {
		checkp->draw(scrollX, scrollY);// Ampliación- Scrollx
	}
	


	for (auto const& projectile : projectiles) {
		projectile->draw(scrollX, scrollY);// Ampliación- Scrollx
	}
	cup->draw(scrollX,scrollY);// Ampliación- Scrollx
	player->draw(scrollX, scrollY);// Ampliación- Scrollx
	for (auto const& enemy : enemies) {
		enemy->draw(scrollX, scrollY);// Ampliación- Scrollx
	}

	backgroundPoints->draw();
	textPoints->draw();
	//AMPLIACIÓN-recolectables
	backgroundCollectables->draw();
	textCollectables->draw();
	// HUD
	if (game->input == game->inputMouse) {
		buttonJump->draw(); // NO TIENEN SCROLL, POSICION FIJA
		buttonShoot->draw(); // NO TIENEN SCROLL, POSICION FIJA
		pad->draw(); // NO TIENEN SCROLL, POSICION FIJA
	}
	if (pause) {
		message->draw();
	}

	SDL_RenderPresent(game->renderer); // Renderiza
}

void GameLayer::gamePadToControls(SDL_Event event) {

	// Leer los botones
	bool buttonA = SDL_GameControllerGetButton(gamePad, SDL_CONTROLLER_BUTTON_A);
	bool buttonB = SDL_GameControllerGetButton(gamePad, SDL_CONTROLLER_BUTTON_B);
	// SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_BUTTON_B
	// SDL_CONTROLLER_BUTTON_X, SDL_CONTROLLER_BUTTON_Y
	cout << "botones:" << buttonA << "," << buttonB << endl;
	int stickX = SDL_GameControllerGetAxis(gamePad, SDL_CONTROLLER_AXIS_LEFTX);
	cout << "stickX" << stickX << endl;

	// Retorna aproximadamente entre [-32800, 32800], el centro debería estar en 0
	// Si el mando tiene "holgura" el centro varia [-4000 , 4000]
	if (stickX > 4000) {
		controlMoveX = 1;
	}
	else if (stickX < -4000) {
		controlMoveX = -1;
	}
	else {
		controlMoveX = 0;
	}

	if (buttonA) {
		controlShoot = true;
	}
	else {
		controlShoot = false;
	}

	if (buttonB) {
		controlMoveY = -1; // Saltar
	}
	else {
		controlMoveY = 0;
	}
}


void GameLayer::mouseToControls(SDL_Event event) {
	// Modificación de coordenadas por posible escalado
	float motionX = event.motion.x / game->scaleLower;
	float motionY = event.motion.y / game->scaleLower;
	// Cada vez que hacen click
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		controlContinue = true;
		if (pad->containsPoint(motionX, motionY)) {
			pad->clicked = true;
			// CLICK TAMBIEN TE MUEVE
			controlMoveX = pad->getOrientationX(motionX);
		}
		if (buttonShoot->containsPoint(motionX, motionY)) {
			controlShoot = true;
		}
		if (buttonJump->containsPoint(motionX, motionY)) {
			controlMoveY = -1;
		}

	}
	// Cada vez que se mueve
	if (event.type == SDL_MOUSEMOTION) {
		if (pad->clicked && pad->containsPoint(motionX, motionY)) {
			controlMoveX = pad->getOrientationX(motionX);
			// Rango de -20 a 20 es igual que 0
			if (controlMoveX > -20 && controlMoveX < 20) {
				controlMoveX = 0;
			}

		}
		else {
			pad->clicked = false; // han sacado el ratón del pad
			controlMoveX = 0;
		}
		if (buttonShoot->containsPoint(motionX, motionY) == false) {
			controlShoot = false;
		}
		if (buttonJump->containsPoint(motionX, motionY) == false) {
			controlMoveY = 0;
		}

	}
	// Cada vez que levantan el click
	if (event.type == SDL_MOUSEBUTTONUP) {
		if (pad->containsPoint(motionX, motionY)) {
			pad->clicked = false;
			// LEVANTAR EL CLICK TAMBIEN TE PARA
			controlMoveX = 0;
		}

		if (buttonShoot->containsPoint(motionX, motionY)) {
			controlShoot = false;
		}
		if (buttonJump->containsPoint(motionX, motionY)) {
			controlMoveY = 0;
		}

	}
}


void GameLayer::keysToControls(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		int code = event.key.keysym.sym;
		// Pulsada
		switch (code) {
		case SDLK_ESCAPE:
			game->loopActive = false;
			break;
		case SDLK_1:
			game->scale();
			break;
		case SDLK_d: // derecha
			controlMoveX = 1;
			break;
		case SDLK_a: // izquierda
			controlMoveX = -1;
			break;
		case SDLK_w: // arriba
			controlMoveY = -1;
			break;
		case SDLK_s: // abajo
			controlMoveY = 1;
			break;
		case SDLK_SPACE: // dispara
			controlShoot = true;
			break;
		}


	}
	if (event.type == SDL_KEYUP) {
		int code = event.key.keysym.sym;
		// Levantada
		switch (code) {
		case SDLK_d: // derecha
			if (controlMoveX == 1) {
				controlMoveX = 0;
			}
			break;
		case SDLK_a: // izquierda
			if (controlMoveX == -1) {
				controlMoveX = 0;
			}
			break;
		case SDLK_w: // arriba
			if (controlMoveY == -1) {
				controlMoveY = 0;
			}
			break;
		case SDLK_s: // abajo
			if (controlMoveY == 1) {
				controlMoveY = 0;
			}
			break;
		case SDLK_SPACE: // dispara
			controlShoot = false;
			break;
		}

	}

}

