#pragma once
#include"Shader.h"
#include"texture.h"
#include"GameObject.h"

#include<vector>

struct Particle {
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particle():Position(0.0f),Velocity(0.0f),Color(1.0f),Life(0.0f){}
};

class ParticleGenerator
{
public:
	ParticleGenerator(Shader& shader, Texture2D texture, GLuint amount);

	//更新所有粒子
	void Update(GLfloat dt, GameObject& go, GLuint newParticles, glm::vec2 offset = glm::vec2(0));

	void Draw();

private:
	std::vector<Particle>particles;
	GLuint amount;
	std::vector<glm::vec2>offset;
	std::vector<glm::vec4>color;
	Shader shader;
	Texture2D texture;
	GLuint VAO;
	/// <summary>
	/// 初始化顶点信息和缓存
	/// </summary>
	void Init();

	int FirstUnusedParticleIndex();

	void RespawnParticle(Particle &particle,GameObject &object,glm::vec2 offset=glm::vec2(0));
};

