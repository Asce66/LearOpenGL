#pragma once
#include"Shader.h"
#include"TextRenderer.h"

#include<map>

struct Character {
	GLuint TextureID;
	glm::vec2 Size;
	glm::vec2 Bearing;
	GLuint Advance;
};

class TextRenderer
{
public:
	std::map<GLchar, Character>Characters;
	Shader TextShader;
	TextRenderer(GLuint width, GLuint heigth);
	void Load(std::string font, GLuint fontSize);
	void RenderText(std::string text, glm::vec2 position,GLfloat scale, glm::vec3 color = glm::vec3(1));
private:
	GLuint VAO, VBO;
};

