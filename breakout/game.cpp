#include "game.h"

#include "resourcemanager.h"

Game::Game(GLuint width, GLuint height)
	: State(GAME_ACTIVE)
	, Keys()
	, Width(width)
	, Height(height)
{

}

Game::~Game()
{
	delete m_renderer;
	delete m_player;
}

void Game::Init()
{
	// Load shaders
	ResourceManager::LoadShader("shaders/vert_sprite.glsl", "shaders/frag_sprite.glsl", nullptr, "sprite");
	
	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0, true);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection, true);
	
	// Load textures
	ResourceManager::LoadTexture("textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("textures/paddle.png", GL_TRUE, "paddle");

	// Set render specific controls
	m_renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// Load levels
	GameLevel one;
	GameLevel two;
	GameLevel three;
	GameLevel four;
	one.Load("levels/one.txt", this->Width, this->Height * 0.5);
	two.Load("levels/two.txt", this->Width, this->Height * 0.5);
	three.Load("levels/three.txt", this->Width, this->Height * 0.5);
	four.Load("levels/four.txt", this->Width, this->Height * 0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->CurrentLevel = 0;

	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	m_player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}

void Game::Update(GLfloat deltaTime)
{

}

void Game::ProcessInput(GLfloat deltaTime)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * deltaTime;

		if (this->Keys[GLFW_KEY_A])
		{
			if (m_player->Position.x >= 0)
			{
				m_player->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (m_player->Position.x <= this->Width - m_player->Size.x)
			{
				m_player->Position.x += velocity;
			}
		}
	}
}

void Game::Render()
{
	m_renderer->DrawSprite(ResourceManager::GetTexture("face"),
		glm::vec2(200, 200), glm::vec2(300, 400), 0.0f, glm::vec3(1.0f, 0.5f, 0.0f));

	this->Levels[this->CurrentLevel].Draw(*m_renderer);

	m_player->Draw(*m_renderer);
}