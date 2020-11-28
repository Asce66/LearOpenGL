#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"../HelloTrangle/CAMER_H.h"
#include"../TriangleWork/SHADER_H.h"


#define STB_IMAGE_IMPLEMENTATION//����������Ԥ�������޸�ͷ�ļ�,����ֻ������صĺ�������Դ��.�����ǰ�ͷ�ļ���Ϊ��.cpp
#include"stb_image.h"

#include<iostream>
#include<vector>
#include<map>

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

float lastX = SCREEN_WIDTH / 2.0f, lastY = SCREEN_HEIGHT / 2.0f;
bool isFirstCursor = true;
Camera camera(glm::vec3(0, 0, 3.0f));

//ʱ��
float lastFrame = 0, deltaTime = 0;

//���ӻ����ڴ�С�ı�ʱ�Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int weight, int height)
{
	glViewport(0, 0, weight, height);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (isFirstCursor) {
		isFirstCursor = false;
		lastX = xPos;
		lastY = yPos;
	}
	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScorll(yoffset);
}

unsigned int LoadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, heigth, nrComponent;

	unsigned char* data = stbi_load(path, &width, &heigth, &nrComponent, 0);
	if (data)
	{
		GLenum format;
		if (nrComponent == 1)
			format = GL_RED;
		else if (nrComponent == 3)
			format = GL_RGB;
		else if (nrComponent == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, heigth, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ͼƬ����ʧ��";
	}
	stbi_image_free(data);
	return textureID;
}

unsigned int LoadCubeMap(std::vector<std::string>& paths)
{
	unsigned int cubeMapID;
	glGenTextures(1, &cubeMapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
	int width, heigth, nrChannels;
	for (int i = 0; i < paths.size(); i++)
	{
		unsigned char* data = stbi_load(paths[i].c_str(), &width, &heigth, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "ͼƬ����ʧ�ܣ�" << paths[i] << std::endl;
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return cubeMapID;
}

//��鰴���Ƿ���
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_W))
		camera.ProcessKeyBoard(FORWARD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_S))
		camera.ProcessKeyBoard(BACKWORD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_A))
		camera.ProcessKeyBoard(LEFT, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_D))
		camera.ProcessKeyBoard(RIGHT, deltaTime);
}

int main()
{
#pragma region ���ô���

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//�������汾��Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//���ôΰ汾��Ϊ3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//������ȾģʽΪ����ģʽ
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//Mac OS Xϵͳ��Ҫ��һ��
#endif // __APPLE__

	//���һ�����ڶ��� ����:��� �߶� ����title ������ ������
	GLFWwindow* window = glfwCreateWindow(800, 600, "HelloOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "����һ�����ڶ���ʧ����\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);//�����ڵ�����������Ϊ��ǰ�̵߳�������

	//GLAD�����ڹ���OpenGL�ĺ���ָ��� �����ڵ����κ�OpenGL����ǰ����Ҫ��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "GLAD����ʧ��\n";
		return 1;
	}
	glEnable(GL_DEPTH_TEST);

	//���ÿ��ӻ����ڵĴ�С ����:���ӻ��������½����� ��� �߶�(����)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//ע����ӻ����ڴ�С�仯ʱ�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	//�������ģʽΪ����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#pragma endregion

#pragma region ��������
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	//��պе�λ������(1X1X1�������壬��������������������uvֵ�͵�һ�õ��λ������)
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	std::vector<glm::vec3> windows
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	//��cube������
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	//����պ������������
	unsigned int skyBoxVAO, skyBoxVBO;
	glGenVertexArrays(1, &skyBoxVAO);
	glGenBuffers(1, &skyBoxVBO);

	glBindVertexArray(skyBoxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
#pragma endregion

	Shader shader("reflectvert.txt", "reflectfrag.txt");
	Shader skyBoxShader("skyboxvert.txt", "skyboxfrag.txt");

	unsigned int cubeTexture = LoadTexture("D:\\_LearOpenGL\\Project1\\Texture\\container2.png");

	//��պе�ÿһ������ͼ·��
	std::vector<std::string> faces
	{
		"D:\\_LearOpenGL\\Project1\\Texture\\skybox\\right.jpg",
		"D:\\_LearOpenGL\\Project1\\Texture\\skybox\\left.jpg",
		"D:\\_LearOpenGL\\Project1\\Texture\\skybox\\top.jpg",
		"D:\\_LearOpenGL\\Project1\\Texture\\skybox\\bottom.jpg",
		"D:\\_LearOpenGL\\Project1\\Texture\\skybox\\front.jpg",
		"D:\\_LearOpenGL\\Project1\\Texture\\skybox\\back.jpg"
	};
	unsigned int cubeMapTexture = LoadCubeMap(faces);

	shader.use();
	shader.setInt("cubeMap", 0);

	skyBoxShader.use();
	skyBoxShader.setInt("cubeMap", 0);
	glm::mat4 v3 = glm::mat4(1);
	v3=glm::transpose(glm::inverse(v3));
#pragma region Uniform�������
	//��ȡ����
	unsigned int uniformBlockIndexReflect = glGetUniformBlockIndex(shader.ID, "Matrices");
	unsigned int uniformBlockIndexSkyBox = glGetUniformBlockIndex(skyBoxShader.ID, "Matrices");

	//������ͬһ��OpenGL�����İ󶨵�(�����ǰ󶨵�0)
	glUniformBlockBinding(shader.ID, uniformBlockIndexReflect, 0);
	glUniformBlockBinding(skyBoxShader.ID, uniformBlockIndexSkyBox, 0);

	//����Uniform�������
	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);

	//�󶨻������
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);//��ԭ
	//���仺������ڴ棬���ð󶨵㣨������0��
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

#pragma endregion

#pragma region ѭ����Ⱦ

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;

		processInput(window);

		//glEnable(GL_DEPTH_TEST);

		//����Ҫ��Ⱦ��͸�����尴�վ�����������ȾԶ������Ⱦ���ġ���֤͸�����岻ͬ�ڵ�ס�����͸������
		std::map<float, glm::vec3>sortedWindows;
		for (int i = 0; i < windows.size(); i++) {
			float distance = glm::length(camera.Position - windows[i]);
			sortedWindows[distance] = windows[i];
		}

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//����Uniform�����������
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),glm::value_ptr(camera.GetViewMatrix()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//��Ⱦ����
		shader.use();
		shader.setVec3("camerPos", camera.Position);
		glm::mat4 model = glm::mat4(1);

		//������
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeMapTexture);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//��պ�

		//���ֵС�ڵ��ھͿ���ͨ����Ȳ��� ��Ϊ�����ֶ�����պе����
		//����Ϊ1��ֻҪ�л������� ��ô��ȶ�����С,������պ�ʱ����������
		//Ȼ�����û�л�������ĵط�,���ֵ���ܵ���1 �����Ȳ��Ժ���Ӧ��<=
		//ʹ����պ���û������ĵط�ͨ����Ȳ��� �ɹ�������
		glDepthFunc(GL_LEQUAL);
		skyBoxShader.use();		
		//ͨ��ȡ��ͼ��������Ͻ�3X3�������Ƴ�λ��(��������ת)���� ֮����ת��4X4
		skyBoxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
		skyBoxShader.setMat4("projection", projection);

		glBindVertexArray(skyBoxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		//��Ȳ��Թ�ΪĬ�ϵ�<
		glDepthFunc(GL_LESS);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyBoxVAO);

	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &skyBoxVBO);

	glfwTerminate();
	return 0;
}