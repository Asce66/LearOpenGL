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
	//�����µ�����
	for (int i = 0; i < newParticles; i++)
	{
		int unusedInd = FirstUnusedParticleIndex();
		RespawnParticle(particles[unusedInd], go, offset);
	}
	//������������
	for (int i = 0; i < amount; i++)
	{
		Particle& p = particles[i];//�������õ�����
		p.Life -= dt;
		if (p.Life > 0)
		{
			p.Position -= p.Velocity * dt;//�ƶ�λ��
			p.Color.a -= dt * 2.5;//����͸����
		}
	}
}

void ParticleGenerator::Draw()
{
	//�޸�͸���Ļ�Ϲ�ʽ��ʹ���ӵ�����һ�����һ��ƽ���ķ���Ч��
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

	//������Ĭ������������
	for (int i = 0; i < amount; i++)
		particles.push_back(Particle());
#pragma endregion
}

int lastUsedParticle = 0;//��һ��ʹ�õ������±꣨���ڿ��ٲ���һ��û�õ������±꣩
int ParticleGenerator::FirstUnusedParticleIndex()
{
	//����һ��ʹ�õ������±꿪ʼ��Ѱû�õ�����
	for (int i = lastUsedParticle; i < particles.size(); i++)
	{
		if (particles[i].Life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	//����㿪ʼ��Ѱû�õ�����
	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (particles[i].Life <= 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	//�������Ӷ����򷵻ص�һ�����ӣ������γ���������գ���Ҫ�޸�һ��������Ŀ���ߴ��ʱ�䣩
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
