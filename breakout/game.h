#ifndef _game_HG_
#define _game_HG_

#include "gameobject.h"
#include "globals.h"

#include "gamelevel.h"
#include "spriterenderer.h"


#include <glm/glm.hpp>

enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.0f);

// Game holds all game-related state and functinality.
// combines all game-related data into a single class for
// easy access to each of the components
class Game
{
public:
	GameState				State;
	GLboolean				Keys[1024];
	GLuint					Width;
	GLuint					Height;
	std::vector<GameLevel>	Levels;
	GLuint					CurrentLevel;


	Game(GLuint width, GLuint height);
	~Game();

	void Init();

	void ProcessInput(GLfloat deltaTime);
	void Update(GLfloat deltaTime);
	void Render();

private:
	SpriteRenderer* m_renderer;
	GameObject* m_player;
};

#endif