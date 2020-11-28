#include<glad/glad.h>//好像是先inglad再inglfw
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

//检查按键是否按下
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
}

//可视化窗口大小改变时的回调函数
void framebuffer_size_callback(GLFWwindow* window, int weight, int height)
{
	glViewport(0, 0, weight, height);
}

//每一个字符的数据
struct Character {
	GLuint textureID;
	glm::vec2 size;//字符大小
	glm::vec2 bearing;//从基准线到字符的左边/顶部偏移值
	GLuint advance;//字符中心距下一个字符中心的距离
};

std::map<GLchar, Character>characters;

GLuint VAO, VBO;

//渲染一段文字
void RenderText(Shader& s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	s.use();
	s.setVec3("texColor",color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	//遍历字符
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat width = ch.size.x * scale;
		GLfloat heigth = ch.size.y * scale;

		//对每一个字符更新VBO
		GLfloat vertexs[6][4] = {
			 { xpos,     ypos + heigth,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + width, ypos,       1.0, 1.0 },

			{ xpos,     ypos + heigth,   0.0, 0.0 },
			{ xpos + width, ypos,       1.0, 1.0 },
			{ xpos + width, ypos + heigth,   1.0, 0.0 }
		};
		//更新VBO内容
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexs),vertexs);
		//设置文本贴图
		glBindTexture(GL_TEXTURE_2D, ch.textureID);		
		//绘制字符
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// 更新位置到下一个字形的原点，注意单位是1/64像素
		x += (ch.advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main()
{ 
#pragma region  创建一个窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置主版本号为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置次版本号为3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//设置渲染模式为核心模式
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//Mac OS X系统需要这一句
#endif // __APPLE__

	//获得一个窗口对象 参数:宽度 高度 窗口title 。。。 。。。
	GLFWwindow* window = glfwCreateWindow(800, 600, "HelloOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "创建一个窗口对象失败了\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);//将窗口的上下文设置为当前线程的上下文

	//GLAD是用于管理OpenGL的函数指针的 所以在调用任何OpenGL函数前都需要初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "GLAD加载失败\n";
		return 1;
	}
	//设置可视化窗口的大小 参数:可视化窗口左下角坐标 宽度 高度(像素)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//注册可视化窗口大小变化时的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//设置鼠标模式为隐藏
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#pragma endregion

#pragma region 字体设置

	std::string fontPath = "D:/_LearOpenGL/Project1/Fonts/FZYTK.TTF";
	FT_Library ft;
	if (FT_Init_FreeType(&ft))//函数出错会返回一个非0整数
		std::cout << "初始化FT_Library失败\n";

	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))//函数出错会返回一个非0整数
		std::cout << "字体加载是吧  " << fontPath << std::endl;
	FT_Set_Pixel_Sizes(face, 0, 48);//宽度为0表示使用给定的高度来动态设定宽度

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//opengl默认为4字节对齐，但是字符每一位是1个字节
	//我们简单的生成128个ASCII码
	for (int i = 0; i < 128; i++)
	{
		//加载字符的字形
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
			std::cout << "加载字符失败: "<<i<<'\n';

		//生成字符的纹理贴图
		GLuint texture;
		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//字形生成的是一个8位灰度图，所以只需要r通道
		//通过face的bitmap，设置纹理的宽，高以及数据(GL_UNSIGNED_BYTE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
			face->glyph->bitmap.width, face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// 设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//存储生成的字符
		Character ch = {
			texture,
			glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters[i] = ch;
	}
	//清理FreeType的资源(注意顺序)
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

#pragma endregion

#pragma region 渲染文本的四边形数据绑定
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, & VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//一个四边形绘制需要六个点，每一个点是一个vec4，所以数据大小为sizeof(float) * 6 * 4
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);//数据需要动态改变来渲染不同的字符
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
#pragma endregion

	//文本渲染需要开启透明的混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//正交矩阵
	glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f,(float)SCREEN_HEIGHT);

	Shader shader("ver.txt", "frag.txt");
	shader.use();
	shader.setInt("text", 0);
	shader.setMat4("projection", projection);
#pragma region 循环渲染流程
	/*循环渲染,在程序关闭前一直绘制图形 不这样设置就会程序渲染一次就停止渲染了*/
	std::cout << glGetError() << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		/*输入*/
		processInput(window);

		/*渲染指令*/
		//清除上次渲染的图形
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//设置清理屏幕颜色
		glClear(GL_COLOR_BUFFER_BIT);//清理屏幕 防止显示上一次渲染的屏幕效果	

		RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		RenderText(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

#pragma region 循环结束处理;
	glfwTerminate(); //释放/删除分配的所有资源
#pragma endregion

	return 0;
}