#include "GameLevel.h"
#include"resource_manager.h"
#include"GameObject.h"
#include<fstream>
#include<sstream>
#include<iostream>

void GameLevel::Load(const char* file, GLuint levelWidth, GLuint levelHeight)
{
	//清空原数据
	Bricks.clear();
	//读取数据
	GLuint tileCode;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<GLuint>>tileData;
	if (fstream)
	{
		while (std::getline(fstream, line))//读取每一行数据
		{
			std::vector<GLuint>row;
			std::istringstream sstream(line);
			while (sstream >> tileCode)//读取被空格分开的每个数字
			{
				row.push_back(tileCode);
			}
			tileData.push_back(row);
		}
		this->Init(tileData, levelWidth, levelHeight);
	}
	else
		std::cout << "关卡数据文本加载错误 " << file << std::endl;
}

void GameLevel::Draw()
{
	shader.Use();
	for (int i = 0; i < amount; i++)
	{
		shader.SetMatrix4(("model[" + std::to_string(i) + "]").c_str(), model[i]);
		shader.SetVector3f(("color[" + std::to_string(i) + "]").c_str(), Bricks[i].Color);
	}
	glActiveTexture(GL_TEXTURE0);
	sprite.Bind();
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
	glBindVertexArray(0);
}

bool GameLevel::IsCompleted()
{
	for (int i = 0; i < Bricks.size(); i++)
		if (!Bricks[i].IsSolid && !Bricks[i].IsDestroied)
			return false;
	return true;
}

void GameLevel::DestoryBlock(int index)
{
	Bricks[index].IsDestroied = true;
	if (amount > 0)
	{
		if (index != amount - 1)
		{
			std::swap(Bricks[index], Bricks[amount - 1]);
			std::swap(model[index], model[amount - 1]);
		}
		amount--;
	}

}

void GameLevel::Init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight)
{
	amount = 0;

	GLuint height = tileData.size();
	if (height <= 0)return;
	GLuint width = tileData[0].size();
	GLfloat unit_height = levelHeight / static_cast<GLfloat>(height);//每一个砖块的高度
	GLfloat unit_width = levelWidth / static_cast<GLfloat>(width);//每一个砖块的宽度
	glm::vec2 size(unit_width, unit_height);

	Texture2D blockSolid = ResourceManager::GetTexture("block_solid");
	Texture2D block = ResourceManager::GetTexture("block");

	model.clear();

	for (GLuint y = 0; y < height; y++)
	{
		for (GLuint x = 0; x < width; x++)
		{
			//判断砖块的类型
			if (tileData[y][x] == 1)
			{
				glm::vec2 pos(x * unit_width, y * unit_height);
				GameObject obj(pos, size, blockSolid, glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = true;
				Bricks.push_back(obj);

				glm::mat4 position(1);
				position = glm::translate(position, glm::vec3(obj.Position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

				position = glm::translate(position, glm::vec3(0.5f * obj.Size.x, 0.5f * obj.Size.y, 0.0f)); // Move origin of rotation to center of quad
				position = glm::rotate(position, obj.Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
				position = glm::translate(position, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

				position = glm::scale(position, glm::vec3(size, 1.0f)); // Last scale

				model.push_back(position);
				amount++;
			}
			else if (tileData[y][x] > 1)
			{
				glm::vec3 aColor(1);
				if (tileData[y][x] == 2)
					aColor = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					aColor = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					aColor = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					aColor = glm::vec3(1.0f, 0.5f, 0.0f);
				glm::vec2 pos = glm::vec2(x * unit_width, y * unit_height);
				GameObject go(pos, size, block, aColor);
				go.IsSolid = false;
				Bricks.push_back(go);

				glm::mat4 position(1);
				position = glm::translate(position, glm::vec3(go.Position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

				position = glm::translate(position, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
				position = glm::rotate(position, go.Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
				position = glm::translate(position, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

				position = glm::scale(position, glm::vec3(size, 1.0f)); // Last scale

				model.push_back(position);
				amount++;
			}

		}
	}

	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	GLuint VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}
