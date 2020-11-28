#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(Shader& ashader, Texture2D atexture, GLuint aamount)
	:shader(ashader), texture(atexture), amount(aamount)
{
	offset.resize(aamount, glm::vec2(0));
	color.resize(aamount, glm::vec4(0));

	Init();
}

void ParticleGenerator::Update(GLfloat dt, GameObject& go, GLuint newParticles, glm::vec2 offset)
{
	//增加新的粒子
	for (int i = 0; i < newParticles; i++)
	{
		int unusedInd = FirstUnusedParticleIndex();
		RespawnParticle(particles[unusedInd], go, offset);
	}
	//更新所有粒子
	for (int i = 0; i < amount; i++)
	{
		Particle& p = particles[i];//这里是用的引用
		p.Life -= dt;
		if (p.Life > 0)
		{
			p.Position -= p.Velocity * dt;//移动位置
			p.Color.a -= dt * 2.5;//降低透明度
		}
	}
}

void ParticleGenerator::Draw()
{
	//修改透明的混合公式，使粒子叠加在一起会有一种平滑的发热效果
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	for (int i = 0; i < particles.size(); i++)
	{
		color[i] = particles[i].Color;
		offset[i] = particles[i].Position;
	}

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	shader.Use();
	for (int i = 0; i <offset.size(); i++)
	{
		shader.SetVector2f(("offset[" + std::to_string(i) + "]").c_str(), offset[i]);
		shader.SetVector4f(("color[" + std::to_string(i) + "]").c_str(), color[i]);
	}

	glBindVertexArray(VAO); 
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
	//for (Particle p : particles)
	//{
	//	if (p.Life > 0)
	//	{
	//		shader.SetVector4f("color", p.Color);
	//		shader.SetVector2f("offset", p.Position);
	//		glActiveTexture(GL_TEXTURE0);
	//		texture.Bind();
	//		glBindVertexArray(VAO);
	//		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//		//glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, amout);
	//		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amout);
	//		//glDrawArrays(GL_TRIANGLES, 0, 6);
	//		glBindVertexArray(0);
	//	}
	//}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::Init()
{
#pragma region VBO
	GLuint VBO;
	GLfloat particle_quad[] = {
	    0.0f, 1.0f, 0.0f, 1.0f,
	    1.0f, 0.0f, 1.0f, 0.0f,
	    0.0f, 0.0f, 0.0f, 0.0f,

	    0.0f, 1.0f, 0.0f, 1.0f,
	    1.0f, 1.0f, 1.0f, 1.0f,
	    1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	//先生成默认数量的粒子
	for (int i = 0; i < amount; i++)
		particles.push_back(Particle());
#pragma endregion
}

int lastUsedParticle = 0;//上一次使用的粒子下标（用于快速查找一个没用的粒子下标）
int ParticleGenerator::FirstUnusedParticleIndex()
{
	//从上一次使用的粒子下标开始搜寻没用的粒子
	for (int i = lastUsedParticle; i < particles.size(); i++)
	{
		if (particles[i].Life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	//从起点开始搜寻没用的粒子
	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (particles[i].Life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	//所有粒子都存活，则返回第一号粒子（如果多次出现这种清空，需要修改一下粒子数目或者存活时间）
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::RespawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
	float random = (rand() % 100 - 50) / 10.0f;
	GLfloat rColor = 0.5 + (rand() % 100) / 100.0f;
	particle.Position = object.Position + random + offset;

	particle.Life = 1.0;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1);
	particle.Velocity = object.Velocity * 0.1f;
}
