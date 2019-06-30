#ifndef _game_HG_
#define _game_HG_

#include "gameobject.h"
#include "globals.h"

#include "spriterenderer.h"
#include "gamelevel.h"
#include "ballobject.h"

#include "particlegenerator.h"
#include "postprocessor.h"

#include <glm/glm.hpp>

enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.0f);

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

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

	void Init(void);

	void ProcessInput(GLfloat deltaTime);
	void Update(GLfloat deltaTime);
	void Render(void);

	void DoCollisions(void);

	void ResetLevel(void);
	void ResetPlayer(void);

private:
	SpriteRenderer* m_renderer;
	GameObject* m_player;
	BallObject* m_ball;
	ParticleGenerator* m_particleGenerator;
	PostProcessor* m_effects;
	float m_shakeTime = 0.0f;
};

#endif