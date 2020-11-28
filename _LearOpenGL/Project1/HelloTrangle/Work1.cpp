#include "Work1.h"
#include<iostream>
using namespace std;
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int weight, int height)
{
	glViewport(0, 0, weight, height);
}

const char* vertexShaderSource = "#version 330 core\n"
"layout(location=0)in vec3 aPos;\n"
"void main()\n"
"{gl_Position=vec4(aPos.x,-aPos.y,aPos.z,1.0);}\0";
const char* fragementShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"//声明了一个uniform的数据,是全局的,是CPU数据传输给GPU的方法
"void main()\n"            //之后可以在程序中动态的设置或改变值
"{FragColor=ourColor;}\0";

int WorkOne::Start()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置主版本号为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置次版本号为3(3.3版本的glfw)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//设置渲染模式为核心模式
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//Mac OS X系统需要这一句
	//获得一个窗口对象 参数:宽度 高度 窗口title 。。。 。。。
	GLFWwindow* window = glfwCreateWindow(800, 600, "HelloOpenGL", NULL, NULL);
	if (window == NULL)
	{
		cout << "创建一个窗口对象失败了\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "加载glad失败\n";
		glfwTerminate();
		return 1;
	}
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int success;
	char infolog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		cout << "顶点着色器编译失败\n" << infolog << endl;
		glfwTerminate();
		return 1;
	}
	unsigned int fragementShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragementShader, 1, &fragementShaderSource, NULL);
	glCompileShader(fragementShader);
	glGetShaderiv(fragementShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragementShader, 512, NULL, infolog);
		cout << "编译片段着色器失败\n" << infolog << endl;
		glfwTerminate();
		return 1;
	}
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragementShader);
	glLinkProgram(shaderProgram);
	glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderProgram, 512, NULL, infolog);
		cout << "链接着色器失败\n" << infolog << endl;
		glfwTerminate();
		return 1;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragementShader);

	float vertex[] = {
	0,0,0,
	0,0.5f,0,
	-0.5f,0,0,
	0,0,0,
	0.5f,0,0,
	0,-0.5f,0
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float timeVal = glfwGetTime();//获取运行秒数
		float greenColor = (sin(timeVal) / 2) + 0.5f;//使值在0-1间变化
		int vertextColorLocation = glGetUniformLocation(shaderProgram, "ourColor");//查询uniform的数据位置(-1即为不存在)
		glUseProgram(shaderProgram);//设置uniform的值前必须使用程序,因为是在当前运行的程序中设置的u值
		glUniform4f(vertextColorLocation, 0, greenColor, 0, 1);//设置uniform的值

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

