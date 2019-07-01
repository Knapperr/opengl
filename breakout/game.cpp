#include "game.h"

#include <algorithm>
#include <sstream>
#include "resourcemanager.h"

Game::Game(GLuint width, GLuint height)
	: State(GAME_MENU)
	, Keys()
	, Width(width)
	, Height(height)
	, CurrentLevel(0)
	, Lives(3)
{

}

Game::~Game()
{
	delete m_renderer;
	delete m_player;
	delete m_ball;
	delete m_particleGenerator;
	delete m_effects;
	delete m_text;
}

void Game::Init(void)
{
	// Load shaders
	ResourceManager::LoadShader("shaders/vert_sprite.glsl", "shaders/frag_sprite.glsl", nullptr, "sprite");
	ResourceManager::LoadShader("shaders/vert_particle.glsl", "shaders/frag_particle.glsl", nullptr, "particle");
	ResourceManager::LoadShader("shaders/vert_post_processing.glsl", "shaders/frag_post_processing.glsl", nullptr, "postprocessing");

	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").SetInteger("image", 0, true);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").SetInteger("sprite", 0, true);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
	
	// Load textures
	ResourceManager::LoadTexture("textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture("textures/particle.png", GL_TRUE, "particle");
	ResourceManager::LoadTexture("textures/powerup_speed.png", GL_TRUE, "powerup_speed");
	ResourceManager::LoadTexture("textures/powerup_sticky.png", GL_TRUE, "powerup_sticky");
	ResourceManager::LoadTexture("textures/powerup_increase.png", GL_TRUE, "powerup_increase");
	ResourceManager::LoadTexture("textures/powerup_confuse.png", GL_TRUE, "powerup_confuse");
	ResourceManager::LoadTexture("textures/powerup_chaos.png", GL_TRUE, "powerup_chaos");
	ResourceManager::LoadTexture("textures/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");

	// Set render specific controls
	m_renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	m_particleGenerator = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	m_effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
	m_text = new TextRenderer(this->Width, this->Height);
	m_text->Load("fonts/OCRAEXT.TTF", 24);


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

	// Player
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	m_player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	// Ball
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	m_ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::Update(GLfloat deltaTime)
{
	m_ball->Move(deltaTime, this->Width);

	this->DoCollisions();

	m_particleGenerator->Update(deltaTime, *m_ball, 2, glm::vec2(m_ball->Radius / 2));

	this->UpdatePowerUps(deltaTime);

	// Reduce the shake time
	if (m_shakeTime > 0.0f)
	{
		m_shakeTime -= deltaTime;
		if (m_shakeTime <= 0.0f)
		{
			m_effects->Shake = false;
		}
	}
	
	// loss condition - did ball reach bottom edge?
	if (m_ball->Position.y >= this->Height)
	{
		--this->Lives;

		// game over
		if (this->Lives == 0)
		{
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	
	// Win condition
	if (this->State == GAME_ACTIVE && this->Levels[this->CurrentLevel].IsCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		m_effects->Chaos = true;
		this->State = GAME_WIN;
	}
}

void Game::Render(void)
{
	// Effects render 
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
	{
		m_effects->BeginRender();	// begin rendering to postprocessing quad
			m_renderer->DrawSprite(ResourceManager::GetTexture("background"),
				glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);

			this->Levels[this->CurrentLevel].Draw(*m_renderer);
			m_player->Draw(*m_renderer);

			for (PowerUp& powerUp : this->PowerUps)
				if (!powerUp.Destroyed)
					powerUp.Draw(*m_renderer);

			m_particleGenerator->Draw();
			m_ball->Draw(*m_renderer);
		// End rendering to postprocessing quad
		m_effects->EndRender();	
		// Render postprocessing quad
		m_effects->Render(glfwGetTime());
		// Render text (don't include in post processing)
		std::stringstream ss; 
		ss << this->Lives;
		m_text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
		std::stringstream currLevel;
		currLevel << this->CurrentLevel;
		m_text->RenderText("Level:" + currLevel.str(), 5.0f, 20.0f, 1.0f);
	}
	if (this->State == GAME_MENU)
	{
		m_text->RenderText("Press ENTER to start", 250.0f, this->Height / 2, 1.0f);
		m_text->RenderText("Press W or S to select level", 245.0f, this->Height / 2 + 20.0f, 0.75f);
	}
	if (this->State == GAME_WIN)
	{
		m_text->RenderText("You WON!!!!", 320.0f, this->Height / 2 - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		m_text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
}

void Game::ProcessInput(GLfloat deltaTime)
{
	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->CurrentLevel = (this->CurrentLevel + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
		}

		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->CurrentLevel > 0)
				--this->CurrentLevel;
			else
				this->CurrentLevel = 3;
		
			this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
		}
	}
	if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			m_effects->Chaos = false;
			this->State = GAME_MENU;
		}
	}
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * deltaTime;

		if (this->Keys[GLFW_KEY_A])
		{
			if (m_player->Position.x >= 0)
			{
				m_player->Position.x -= velocity;
				if (m_ball->Stuck)
				{
					m_ball->Position.x -= velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (m_player->Position.x <= this->Width - m_player->Size.x)
			{
				m_player->Position.x += velocity;
				if (m_ball->Stuck)
				{
					m_ball->Position.x += velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
		{
			m_ball->Stuck = false;
		}
	}
}

void Game::ResetLevel(void)
{
	if (this->CurrentLevel == 0)
	{
		this->Levels[0].Load("levels/one.txt", this->Width, this->Height * 0.5);
	}
	else if (this->CurrentLevel == 1)
	{
		this->Levels[1].Load("levels/two.txt", this->Width, this->Height * 0.5);
	}
	else if (this->CurrentLevel == 2)
	{
		this->Levels[2].Load("levels/three.txt", this->Width, this->Height * 0.5);
	}
	else if (this->CurrentLevel == 3)
	{
		this->Levels[3].Load("levels/four.txt", this->Width, this->Height * 0.5);
	}
}

void Game::ResetPlayer(void)
{
	m_player->Size = PLAYER_SIZE;
	m_player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	m_ball->Reset(m_player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}

// Power ups
// ---------------------------------------------------------------
bool isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

void Game::UpdatePowerUps(float deltaTime)
{
	for (PowerUp& powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * deltaTime;
		if (powerUp.Activated)
		{
			powerUp.Duration -= deltaTime;
			if (powerUp.Duration <= 0.0f)
			{
				// Remove the powerup from the list (will be removed later on)
				powerUp.Activated = false;

				if (powerUp.Type == "sticky")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "sticky"))
					{
						// Only reset if no other powerup of type sticky is active
						m_ball->Sticky = false;
						m_player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "pass-through"))
					{
						m_ball->PassThrough = false;
						m_ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "confuse"))
					{
						m_effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
					{
						m_effects->Chaos = false;
					}
				}
			}
		}
	}

	// Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
	// Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp & powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

bool shouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps(GameObject& block)
{
	if (shouldSpawn(25)) // 1 in 75 chance
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	if (shouldSpawn(10))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	if (shouldSpawn(30))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (shouldSpawn(20))
		this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	if (shouldSpawn(50)) // Negative powerups should spawn more often
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
	if (shouldSpawn(55))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));

}

bool isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	for (const PowerUp &powerUp : powerUps)
	{ 
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}

void Game::activatePowerUp(PowerUp& powerUp)
{
	//Initiate a powerup based on type of powerup
	if (powerUp.Type == "speed")
	{
		m_ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		m_ball->Sticky = true;
		m_player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		m_ball->PassThrough = true;
		m_ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		m_player->Size.x += 100;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!m_effects->Chaos)
			m_effects->Confuse = true; // Only activate if chaos wasn't already active
	}
	else if (powerUp.Type == "chaos")
	{
		if (!m_effects->Confuse)
			m_effects->Chaos = true;
	}
}


// Collision detection
// ---------------------------------------------------------------
bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 target);

void Game::DoCollisions(void)
{
	for (GameObject& box : this->Levels[this->CurrentLevel].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*m_ball, box);
			if (std::get<0>(collision))
			{
				if (!box.IsSolid)
				{
					box.Destroyed = GL_TRUE;
					this->SpawnPowerUps(box);
				}
				else
				{
					// SCREEN SHAKE
					m_shakeTime = 0.05f;
					m_effects->Shake = true;

				}

				// Collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(m_ball->PassThrough && !box.IsSolid))
				{
					if (dir == LEFT || dir == RIGHT) // Horizontal collision
					{
						m_ball->Velocity.x = -m_ball->Velocity.x; // reverse the horizontal velocity

						// Relocate
						float penetration = m_ball->Radius - std::abs(diff_vector.x);

						if (dir == LEFT)
						{
							m_ball->Position.x += penetration; // move the ball to right
						}
						else
						{
							m_ball->Position.x -= penetration; // move the ball to left
						}
					}
					else
					{
						// Vertical Collision
						m_ball->Velocity.y = -m_ball->Velocity.y; // Reverse vertical velocity
						// Relocate
						float penetration = m_ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
						{
							m_ball->Position.y -= penetration; // move the ball to up
						}
						else
						{
							m_ball->Position.y += penetration; // move the ball to down
						}
					}
				}
			}
		}
	}

	// Also check collisions for Powerups 
	for (PowerUp &powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			// check if powerup passed bottom edge, if so: keep it inactive and destroy it
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = true;

			if (CheckCollision(*m_player, powerUp))
			{
				activatePowerUp(powerUp);
				powerUp.Destroyed = true;
				powerUp.Activated = true;
			}
		}
	}

	// Also check collisions for player pad (unless stuck)
	Collision result = CheckCollision(*m_ball, *m_player);
	if (!m_ball->Stuck && std::get<0>(result))
	{
		// Check where it hit the board, and change velocity based on where it hit the board
		float centerBoard = m_player->Position.x + m_player->Size.x / 2;
		float distance = (m_ball->Position.x + m_ball->Radius) - centerBoard;
		float percentage = distance / (m_player->Size.x / 2);
		// Move accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = m_ball->Velocity;
		m_ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		// m_ball->Velocity.y = -m_ball->Velocity.y;
		// Keep the speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
		m_ball->Velocity = glm::normalize(m_ball->Velocity) * glm::length(oldVelocity); 
		// Fix sticky paddle
		m_ball->Velocity.y = -1 * abs(m_ball->Velocity.y);

		// if sticky powerup is activated also stick ball to paddle once new velocity vectors were calculated
		m_ball->Stuck = m_ball->Sticky;
	}
}


bool CheckCollision(GameObject &one, GameObject &two)
{
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
						two.Position.x + two.Size.x >= one.Position.x;

	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
						two.Position.y + two.Size.y >= one.Position.y;

	return collisionX && collisionY;
}

Collision CheckCollision(BallObject& one, GameObject& two)
{
	// Get center point circle first
	glm::vec2 center(one.Position + one.Radius);
	// Calcualte AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
	// Get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// Now that we know the clamped vlaues, add this to AABB_center and we get the value of box closest to the circle
	glm::vec2 closest = aabb_center + clamped;
	// Now retrieve the vector between circle and closest point AABB and check if length < radius
	difference = closest - center;

	// not <= since in that case a collision also occurs when object one exactly touches object two,
	// which they are at the end of each collision resolution stage.
	if (glm::length(difference) < one.Radius)
	{
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	}
	else
	{
		return std::make_tuple(GL_FALSE, Direction::UP, glm::vec2(0, 0));
	}
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),  // up
		glm::vec2(1.0f, 0.0f),  // right
		glm::vec2(0.0f, -1.0f), // down
		glm::vec2(-1.0f, 0.0f)  // left
	};

	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; ++i)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}