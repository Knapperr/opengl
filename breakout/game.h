#ifndef _Game_HG_
#define _Game_HG_

#include "globals.h"

enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Game holds all game-related state and functinality.
// combines all game-related data into a single class for
// easy access to each of the components

class Game
{
public:
	GameState	State;
	GLboolean	Keys[1024];
	GLuint		Width;
	GLuint		Height;

	Game(GLuint width, GLuint height);
	~Game();

	void Init();

	void ProcessInput(GLfloat deltaTime);
	void Update(GLfloat deltaTime);
	void Render();
};

#endif