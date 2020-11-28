#pragma once
#include<glad/glad.h>
#include"GameObject.h"
#include"sprite_renderer.h"
#include<vector>
class GameLevel
{
public:
	std::vector<GameObject>Bricks;
	GameLevel(Shader&s,Texture2D&texture):amount(0),shader(s),sprite(texture) {}
	//���ļ��ж�ȡ�ؿ�����
	void Load(const char* file, GLuint levelWidth, GLuint levelHeight);
	void Draw();
	bool IsCompleted();
	void DestoryBlock(int index);
	int amount;
private:
	//��ש�����ݳ�ʼ���ؿ�
	void Init(std::vector<std::vector<GLuint>>tileData, GLuint levelWidth, GLuint levelHeight);
	Shader shader;
	Texture2D sprite;
	std::vector<glm::mat4>model;
	GLuint VAO;
};

