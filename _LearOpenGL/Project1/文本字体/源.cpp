#include<glad/glad.h>//��������inglad��inglfw
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<ft2build.h>
#include FT_FREETYPE_H
#include"../TriangleWork/SHADER_H.h"

#include<map>
#include<iostream>

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

//��鰴���Ƿ���
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
}

//���ӻ����ڴ�С�ı�ʱ�Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int weight, int height)
{
	glViewport(0, 0, weight, height);
}

//ÿһ���ַ�������
struct Character {
	GLuint textureID;
	glm::vec2 size;//�ַ���С
	glm::vec2 bearing;//�ӻ�׼�ߵ��ַ������/����ƫ��ֵ
	GLuint advance;//�ַ����ľ���һ���ַ����ĵľ���
};

std::map<GLchar, Character>characters;

GLuint VAO, VBO;

//��Ⱦһ������
void RenderText(Shader& s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	s.use();
	s.setVec3("texColor",color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	//�����ַ�
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat width = ch.size.x * scale;
		GLfloat heigth = ch.size.y * scale;

		//��ÿһ���ַ�����VBO
		GLfloat vertexs[6][4] = {
			 { xpos,     ypos + heigth,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + width, ypos,       1.0, 1.0 },

			{ xpos,     ypos + heigth,   0.0, 0.0 },
			{ xpos + width, ypos,       1.0, 1.0 },
			{ xpos + width, ypos + heigth,   1.0, 0.0 }
		};
		//����VBO����
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexs),vertexs);
		//�����ı���ͼ
		glBindTexture(GL_TEXTURE_2D, ch.textureID);		
		//�����ַ�
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// ����λ�õ���һ�����ε�ԭ�㣬ע�ⵥλ��1/64����
		x += (ch.advance >> 6) * scale; // λƫ��6����λ����ȡ��λΪ���ص�ֵ (2^6 = 64)
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main()
{ 
#pragma region  ����һ������
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
	//���ÿ��ӻ����ڵĴ�С ����:���ӻ��������½����� ��� �߶�(����)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//ע����ӻ����ڴ�С�仯ʱ�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//�������ģʽΪ����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#pragma endregion

#pragma region ��������

	std::string fontPath = "D:/_LearOpenGL/Project1/Fonts/FZYTK.TTF";
	FT_Library ft;
	if (FT_Init_FreeType(&ft))//��������᷵��һ����0����
		std::cout << "��ʼ��FT_Libraryʧ��\n";

	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))//��������᷵��һ����0����
		std::cout << "��������ǰ�  " << fontPath << std::endl;
	FT_Set_Pixel_Sizes(face, 0, 48);//���Ϊ0��ʾʹ�ø����ĸ߶�����̬�趨���

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//openglĬ��Ϊ4�ֽڶ��룬�����ַ�ÿһλ��1���ֽ�
	//���Ǽ򵥵�����128��ASCII��
	for (int i = 0; i < 128; i++)
	{
		//�����ַ�������
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
			std::cout << "�����ַ�ʧ��: "<<i<<'\n';

		//�����ַ���������ͼ
		GLuint texture;
		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//�������ɵ���һ��8λ�Ҷ�ͼ������ֻ��Ҫrͨ��
		//ͨ��face��bitmap����������Ŀ����Լ�����(GL_UNSIGNED_BYTE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
			face->glyph->bitmap.width, face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// ��������ѡ��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//�洢���ɵ��ַ�
		Character ch = {
			texture,
			glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters[i] = ch;
	}
	//����FreeType����Դ(ע��˳��)
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

#pragma endregion

#pragma region ��Ⱦ�ı����ı������ݰ�
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, & VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//һ���ı��λ�����Ҫ�����㣬ÿһ������һ��vec4���������ݴ�СΪsizeof(float) * 6 * 4
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);//������Ҫ��̬�ı�����Ⱦ��ͬ���ַ�
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
#pragma endregion

	//�ı���Ⱦ��Ҫ����͸���Ļ��
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//��������
	glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f,(float)SCREEN_HEIGHT);

	Shader shader("ver.txt", "frag.txt");
	shader.use();
	shader.setInt("text", 0);
	shader.setMat4("projection", projection);
#pragma region ѭ����Ⱦ����
	/*ѭ����Ⱦ,�ڳ���ر�ǰһֱ����ͼ�� ���������þͻ������Ⱦһ�ξ�ֹͣ��Ⱦ��*/
	std::cout << glGetError() << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		/*����*/
		processInput(window);

		/*��Ⱦָ��*/
		//����ϴ���Ⱦ��ͼ��
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//����������Ļ��ɫ
		glClear(GL_COLOR_BUFFER_BIT);//������Ļ ��ֹ��ʾ��һ����Ⱦ����ĻЧ��	

		RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		RenderText(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

#pragma region ѭ����������;
	glfwTerminate(); //�ͷ�/ɾ�������������Դ
#pragma endregion

	return 0;
}