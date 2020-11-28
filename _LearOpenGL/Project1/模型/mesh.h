#ifndef  MESH_H
#define MESH_H
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<vector>

#include"../TriangleWork/SHADER_H.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	
};

struct Texture {
	std::string type;
	unsigned int id;
};

class Mesh {
public:
	//网格的数据
	std::vector<Vertex>vertexs;
	std::vector<unsigned int>indices;
	std::vector<Texture>textures;

	unsigned int VAO;

	/*函数*/
	Mesh(std::vector<Vertex>& vertex, std::vector<unsigned int>& indices, std::vector<Texture>& texture) :
		vertexs(vertex), indices(indices), textures(texture)
	{
		SetupMesh();
	}

	void Draw(const Shader& shader)
	{
		unsigned int diffuseCnt = 1, specularCnt = 1,normalCnt=1;		
		for (int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			std::string name = textures[i].type;
			std::string  number;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseCnt++);
			else if (name == "texture_specular")
			{
				number = std::to_string(specularCnt++);
			}
			else if (name == "texture_normal") {
				number = std::to_string(normalCnt++);
			}
			shader.setInt( name + number, i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		//绘制网格
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

	unsigned int GetVAO() { return VAO; }
private:
	unsigned int VBO, EBO;
	void SetupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(Vertex), &vertexs[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//顶点位置
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		//法线坐标
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		
		//纹理坐标
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		//切线
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

		
		glBindVertexArray(0);
	}
};
#endif

