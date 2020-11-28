#pragma once
#include"GameObject.h"

#include<string>

const glm::vec2 SIZE(60, 20);
const glm::vec2 VELOCITY(0, 150);

class PowerUp:public GameObject
{
public:
	std::string Type;
	GLfloat Duration;
	GLboolean Activated;

	PowerUp(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position,  Texture2D& texture) :
		GameObject(position, SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() {}
};

