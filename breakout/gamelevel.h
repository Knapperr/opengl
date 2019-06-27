#ifndef _gamelevel_HG_
#define _gamelevel_HG_

#include "gameobject.h"
#include <vector>

class GameLevel
{
public:
	std::vector<GameObject> Bricks;

	GameLevel() { }
	
	void Load(const GLchar* file, GLuint levelWidth, GLuint levleHeight);
	void Draw(SpriteRenderer &renderer);
	GLboolean IsCompleted();
private:
	void init(std::vector<std::vector<GLuint>> tileData, GLuint lvlWidth, GLuint lvlHeight);
};

#endif