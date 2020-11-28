#include<glad/glad.h>//好像是先inglad再inglfw
#include<GLFW/glfw3.h>
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION//定义它会让预处理器修改头文件,让其只包含相关的函数定义源码.等于是把头文件变为了.cpp
#include"stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"CAMER_H.h" 
//顶点着色器源码
const char* vertexShaderSource = "#version 440 core\n"//版本声明,对应OpenGL的版本 这里是OpenGL3.3 核心模式
"layout(location=0)in vec3 aPos;\n"//vec3:三维向量 还有vec1到vec4
"//layout(location=1)in vec3 aColor;\n"
"layout(location=1)in vec2 aTexCoord;\n"
"uniform mat4 transform;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model,view,projection;\n"
"void main()\n"
"{\n"
"gl_Position=projection*view*model*vec4(aPos,1.0f);\n"
"TexCoord=aTexCoord;\n"
"//ourColor=aColor;\n"
"}\0";//这里是\0

//片段着色器源码
const char* fragmentShaderSource = "#version 440 core\n"
"out vec4 fragColor;\n"//rgba颜色
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D texture1;\n"//2d纹理采样器
"uniform sampler2D texture2;\n"
"void main()\n"
"{\n"//mix函数有混合的作用,0.2表示第一个元素占0.8,第二个占0.2
"fragColor=mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2);\n"
"}\0";//这里是\0   

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
#pragma region  摄像机相关
Camera camera(glm::vec3(0,0,3));//相机初始位置为0,0,3
float lastX = SCREEN_WIDTH / 2.0f, lastY = SCREEN_HEIGHT / 2.0f;
bool isFirstCurse = true;//是否是第一次检测到鼠标
float deltaTime=0, lastFrame=0;
#pragma endregion

//检查按键是否按下
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W))
		camera.ProcessKeyBoard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S))
		camera.ProcessKeyBoard(BACKWORD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A))
		camera.ProcessKeyBoard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D))
		camera.ProcessKeyBoard(RIGHT, deltaTime);	
}

//鼠标位置改变回调
void mouse_callBack(GLFWwindow* window, double posx, double posy) {
	if (isFirstCurse) {
		lastX = posx;
		lastY = posy;
		isFirstCurse = false;
	}
	float offsetx = posx - lastX, offsety = lastY-posy;//y偏移是相反的
	lastX = posx; lastY = posy;
	camera.ProcessMouseMovement(offsetx, offsety);
}

//滑轮滑动回调
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScorll(yoffset);
}

//可视化窗口大小改变时的回调函数
void framebuffer_size_callback(GLFWwindow* window, int weight, int height)
{
	glViewport(0, 0, weight, height);
}

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

int main()
{
#pragma region  创建一个窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//设置主版本号为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);//设置次版本号为3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//设置渲染模式为核心模式
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);//发布的时候需要不这里注释掉，节省性能
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//Mac OS X系统需要这一句
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
	GLint flag;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flag);
	if (flag & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);//同步的输出消息
		glDebugMessageCallback(glDebugOutput, nullptr);//注册错误输出回调
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	//设置可视化窗口的大小 参数:可视化窗口左下角坐标 宽度 高度(像素)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//注册可视化窗口大小变化时的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//设置鼠标模式为隐藏
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callBack);
	glfwSetScrollCallback(window, scroll_callback);
#pragma endregion

#pragma region 创建着色器  
	/*顶点着色器*/
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//创建一个顶点着色器对象
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//设置顶点着色器源码
	glCompileShader(vertexShader);//编译顶点着色器源码
	//检查glCompileShader是否编译成功
	int success;
	char infolog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//获得Shader编译结果
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);//获得编译错误信息
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infolog << std::endl;
	}

	/*片段着色器(Fragment Shader) //与顶点着色器大同小异*/
	unsigned int fragementShader;
	fragementShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragementShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragementShader);
	glGetShaderiv(fragementShader, GL_COMPILE_STATUS, &success);//获得Shader编译结果
	if (!success)
	{
		glGetShaderInfoLog(fragementShader, 512, NULL, infolog);//获得编译错误信息
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infolog << std::endl;
	}
#pragma endregion

#pragma region 链接着色器
	/*着色器程序*/
	unsigned int shaderPragram;
	shaderPragram = glCreateProgram();//创建一个着色器并返回它的ID引用
	glAttachShader(shaderPragram, vertexShader);//附加着色器
	glAttachShader(shaderPragram, fragementShader);
	glLinkProgram(shaderPragram);//链接着色器
	//检查着色器链接是否成功
	glGetProgramiv(shaderPragram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderPragram, 512, NULL, infolog);
		std::cout << "链接着色器失败\n" << infolog << std::endl;
	}
	glDeleteShader(vertexShader);//链接完成后就不需要着色器了 删除它们
	glDeleteShader(fragementShader);
#pragma endregion

#pragma region 设置顶点数据生成数组对象和缓存/索引缓存对象
	float vertexs1[] = {//三角形的三个顶点位置坐标与颜色信息
	//顶点	  
	-0.5,-0.5,0,
	0.5,-0.5,0,
	0,0.5,0,
	};
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float vertexs2[] = {//四边形的顶点数据
	//--位置--        --颜色   --纹理uv坐标
	0.5f, 0.5f, 0.0f,  1,0,0,	1,1,	// 右上角
	0.5f, -0.5f,0.0f,  0,1,0,	1,0,	// 右下角
	-0.5f,-0.5f,0.0f,  0,0,1,	0,0	,// 左下角
	-0.5f, 0.5f,0.0f,  1,1,0,	0,1	// 左上角	
	};
	unsigned int indices[] = {//用2个三角形绘制出一个四边形(索引从0开始)
		0, 1, 3, // 第一个三角形
	1, 2, 3  // 第二个三角形
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);//创建顶点数组对象 与顶点缓存对象类似
	glGenBuffers(1, &VBO);//生成一个顶点缓存对象 参数1为该对象的ID,时独一无二的
	glGenBuffers(1, &EBO);//创建缓存对象
	glBindVertexArray(VAO);//绑定顶点数据对象(在缓存对象之前绑定,才能正常使用缓存对象)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//OpenGL有很多缓冲对象类型，顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER。
									   //OpenGL允许我们同时绑定多个缓冲，只要它们是不同的缓冲类型。使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上：
	//把顶点数据复制到缓存对象中 参数四的三种形式:GL_STATIC_DRAW ：数据不会或几乎不会改变。
												//GL_DYNAMIC_DRAW：数据会被改变很多。
												//GL_STREAM_DRAW ：数据每次绘制时都会改变。
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//绑定一个索引缓存对象,必须和顶点缓存对象联合使用,目的是节约重复的顶点数据空间
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	/*链接顶点属性*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	/*
	glVertexAttribPointer参数：
	1.指定要配置的顶点属性 上面的着色器源码设置了layout(location=0) 所以这里是0与之对应
	2.顶点属性大小 顶点是一个vec3,有三个值,所以这里是3
	3.顶点数据类型 这里是float型
	4.下个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
	5.参数步长,即连续的顶点数据组之间距离 每个顶点都是3个float,所以这样设置
	6.表示位置数据在缓冲中起始位置的偏移量。由于位置数据在数组的开头，所以这里是0
	*/
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
#pragma endregion
	/*glUseProgram(shaderPragram);//设置uniform的值
	glUniform1f(glGetAttribLocation(shaderPragram, "offset"), 30);*/

#pragma region 添加纹理贴图
	//创建texture并绑定
	/*第一张贴图*/
	unsigned int texture1;
	glGenTextures(1, &texture1);//1.生成纹理数量 2.纹理存储id
	glBindTexture(GL_TEXTURE_2D, texture1);
	//为当前绑定的纹理设置环绕、过滤方式
	//贴图的uv坐标轴是按str区分的,对应xyz.坐标轴从左下角开始,从轴上值为0-1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//使用stb_image加载第一张图片
	int width1, height1, nChannels1;
	const char* fileName1 = "D:\\_LearOpenGL\\Project1\\Texture\\container.jpg";
	unsigned char* data1 = stbi_load(fileName1, &width1, &height1, &nChannels1, 0);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		/*参数:
			1.纹理目标
			2.指定多级渐远纹理级别,希望手动设置每个多级渐远纹理级别,就设为0,即为基本级别
			3.OpenGL把纹理存储为何种格式
			4.5.宽度和高度
			6.。。。。
			7.原图格式
			8.数据类型.
			9.图像数据
		*/
		glGenerateMipmap(GL_TEXTURE_2D);//自动为当前绑定的纹理生成多级渐变纹理
	}
	else {
		std::cout << "图片加载错误: " << std::endl;
	}
	stbi_image_free(data1);//释放数据

	/*第二张贴图*/
	unsigned int texture2;
	glGenTextures(1, &texture2);//1.生成纹理数量 2.纹理存储id
	glBindTexture(GL_TEXTURE_2D, texture2);
	//为当前绑定的纹理设置环绕、过滤方式
	//贴图的uv坐标轴是按str区分的,对应xyz.坐标轴从左下角开始,从轴上值为0-1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//使用stb_image加载第二张图片
	int width2, height2, nChannels2;
	const char* fileName2 = "D:\\_LearOpenGL\\Project1\\Texture\\awesomeface.png";
	stbi_set_flip_vertically_on_load(true);//加载后翻转图片y轴
	unsigned char* data2 = stbi_load(fileName2, &width2, &height2, &nChannels2, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		/*参数:
			1.纹理目标
			2.指定多级渐远纹理级别,希望手动设置每个多级渐远纹理级别,就设为0,即为基本级别
			3.OpenGL把纹理存储为何种格式
			4.5.宽度和高度
			6.。。。。
			7.原图格式
			8.数据类型.
			9.图像数据
		*/
		glGenerateMipmap(GL_TEXTURE_2D);//自动为当前绑定的纹理生成多级渐变纹理
	}
	else {
		std::cout << "图片加载错误: " << std::endl;
	}
	stbi_image_free(data2);//释放数据

	glUseProgram(shaderPragram);
	glUniform1i(glGetUniformLocation(shaderPragram, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderPragram, "texture2"), 1);
#pragma endregion

#pragma region 矩阵变化成为三维物体
	/*设置模型矩阵(局部坐标转世界坐标用) 视野矩阵(世界坐标转观察坐标) 透视矩阵(观察坐标转裁剪坐标)*/
	glm::mat4 model = glm::mat4(1);
	//必须写.0f 效果:一个模型矩阵,绕x轴旋转
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1, 0, 0));
	glm::mat4 view = glm::mat4(1);
	//摄像机向后移动等于整个场景向前移动(即为摄像机的视口方向为摄像机本身坐标的-z)
	view = glm::translate(view, glm::vec3(0, 0, -3));
	glm::mat4 projection = glm::mat4(1);
	//透射投影矩阵 1.视野大小 2.视野宽高比 3.近平面距离 4.远平面距离
	projection = glm::perspective(glm::radians(45.0f), 800 / 600.0f, 0.1f, 100.0f);
	/*设置矩阵给顶点着色器*/
	glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glEnable(GL_DEPTH_TEST);//开启z深度测试(默认没开启),之后就一直开启,直到调用glDisable
	//之后还要在glClear函数中指定清除清度缓冲（否则前一帧的深度信息仍然保存在缓冲中）
	//glDisable(GL_DEPTH_TEST);//关闭深度测试
	/*更多图形的位置信息*/
	glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
	};
#pragma endregion

	/* //旋转/缩放物体
	glm::mat4 trans = glm::mat4(1);
	trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0, 0, 1));
	trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
	glUseProgram(shaderPragram);
	glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
	*/
#pragma region 循环渲染流程
	/*循环渲染,在程序关闭前一直绘制图形 不这样设置就会程序渲染一次就停止渲染了*/
	while (!glfwWindowShouldClose(window))
	{
		/*输入*/
		processInput(window);
		/*渲染指令*/
		//清除上次渲染的图形
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//设置清理屏幕颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清理屏幕 防止显示上一次渲染的屏幕效果
		//渲染三角形
		glActiveTexture(GL_TEXTURE0);//一个纹理即为1个纹理单元,当只绑定了一个纹理时,默认纹理单元编号为0,是自动激活的
		//OpenGL有16个纹理单元(0-15)
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUseProgram(shaderPragram);
		glBindVertexArray(VAO);
		for (int i = 0; i < 10; i++) {
			glm::mat4 mode = glm::mat4(1);
			mode = glm::translate(mode, cubePositions[i]);
			float angle = 20 * i;
			mode = glm::rotate(mode, glm::radians(angle), glm::vec3(1, 0.3f, 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "model"), 1, GL_FALSE, glm::value_ptr(mode));
			glDrawArrays(GL_TRIANGLES, 0, 36);//使用的顶点缓存对象调用它
		}
		glm::mat4 view = glm::mat4(1);
		//float radius = 10.0f;//摄像机环绕物体
		//float clamx = sin(glfwGetTime()) * radius;
		//float clamz = cos(glfwGetTime()) * radius;
		////看向指定位置的矩阵 1.自身位置 2.目标位置 3.目标的上向量
		//view = glm::lookAt(glm::vec3(clamx, 0, clamz), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//使用的索引缓存调用它 6:顶点数目 0:偏移量
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 projection = glm::mat4(1);
		projection = glm::perspective(glm::radians( camera.Zoom), SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		/*检查并调用事件 交换缓冲*/
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

#pragma region 循环结束处理
	glDeleteVertexArrays(1, &VAO);//必须先删除顶点数组对象
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate(); //释放/删除分配的所有资源
#pragma endregion

	return 0;
}