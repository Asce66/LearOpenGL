#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Game.h"
#include"resource_manager.h"
#include<iostream>

const int SCREEN_WIDTH = 800, SCREEN_HEIGTH = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGTH);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//ʱ�����
GLfloat deltaTime = 0;
GLfloat laseFrame = 0;

//���Իص�����
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
#pragma region ��������

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);//���汾�ţ������õ���OpenGL4.4��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//������Ⱦģʽ
	//��ȡ�������������
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);//������ʱ����Ҫ������ע�͵�����ʡ����
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//Mac OS Xϵͳ��Ҫ��һ��
#endif // __APPLE__
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGTH, "BreakOut", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "��������ʧ��\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);//����OpenGL������

	//GLAD�����ڹ���OpenGL�ĺ���ָ��� �����ڵ����κ�OpenGL����ǰ����Ҫ��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "��ʼ��Gladʧ��\n";
		glfwTerminate();
		return 1;
	}

	GLint flag;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flag);
	if (flag & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);//ͬ���������Ϣ
		glDebugMessageCallback(glDebugOutput, nullptr);//ע���������ص�
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH);
	glfwSetKeyCallback(window, key_callback);

#pragma endregion

	Breakout.Init();

	glEnable(GL_BLEND);
	//ѭ����Ⱦ

	while (!glfwWindowShouldClose(window))
	{
		//����֡��
		GLfloat timer = glfwGetTime();
		deltaTime = timer - laseFrame;
		laseFrame = timer;

		glfwPollEvents();

		Breakout.ProcessInput(deltaTime);
		Breakout.Update(deltaTime);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		Breakout.Render();

		glfwSwapBuffers(window);
	}

	ResourceManager::Clear();
	glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			Breakout.Keys[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
		{
			Breakout.Keys[key] = GL_FALSE;
			Breakout.KeysProcessed[key] = GL_FALSE;
		}
			
	}
}
