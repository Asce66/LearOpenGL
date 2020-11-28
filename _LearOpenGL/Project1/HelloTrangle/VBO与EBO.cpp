#include<glad/glad.h>//��������inglad��inglfw
#include<GLFW/glfw3.h>
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION//����������Ԥ�������޸�ͷ�ļ�,����ֻ������صĺ�������Դ��.�����ǰ�ͷ�ļ���Ϊ��.cpp
#include"stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"CAMER_H.h" 
//������ɫ��Դ��
const char* vertexShaderSource = "#version 440 core\n"//�汾����,��ӦOpenGL�İ汾 ������OpenGL3.3 ����ģʽ
"layout(location=0)in vec3 aPos;\n"//vec3:��ά���� ����vec1��vec4
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
"}\0";//������\0

//Ƭ����ɫ��Դ��
const char* fragmentShaderSource = "#version 440 core\n"
"out vec4 fragColor;\n"//rgba��ɫ
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D texture1;\n"//2d���������
"uniform sampler2D texture2;\n"
"void main()\n"
"{\n"//mix�����л�ϵ�����,0.2��ʾ��һ��Ԫ��ռ0.8,�ڶ���ռ0.2
"fragColor=mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2);\n"
"}\0";//������\0   

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
#pragma region  ��������
Camera camera(glm::vec3(0,0,3));//�����ʼλ��Ϊ0,0,3
float lastX = SCREEN_WIDTH / 2.0f, lastY = SCREEN_HEIGHT / 2.0f;
bool isFirstCurse = true;//�Ƿ��ǵ�һ�μ�⵽���
float deltaTime=0, lastFrame=0;
#pragma endregion

//��鰴���Ƿ���
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

//���λ�øı�ص�
void mouse_callBack(GLFWwindow* window, double posx, double posy) {
	if (isFirstCurse) {
		lastX = posx;
		lastY = posy;
		isFirstCurse = false;
	}
	float offsetx = posx - lastX, offsety = lastY-posy;//yƫ�����෴��
	lastX = posx; lastY = posy;
	camera.ProcessMouseMovement(offsetx, offsety);
}

//���ֻ����ص�
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScorll(yoffset);
}

//���ӻ����ڴ�С�ı�ʱ�Ļص�����
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
#pragma region  ����һ������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//�������汾��Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);//���ôΰ汾��Ϊ3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//������ȾģʽΪ����ģʽ
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);//������ʱ����Ҫ������ע�͵�����ʡ����
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//Mac OS Xϵͳ��Ҫ��һ��
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
	GLint flag;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flag);
	if (flag & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);//ͬ���������Ϣ
		glDebugMessageCallback(glDebugOutput, nullptr);//ע���������ص�
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	//���ÿ��ӻ����ڵĴ�С ����:���ӻ��������½����� ��� �߶�(����)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//ע����ӻ����ڴ�С�仯ʱ�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//�������ģʽΪ����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callBack);
	glfwSetScrollCallback(window, scroll_callback);
#pragma endregion

#pragma region ������ɫ��  
	/*������ɫ��*/
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//����һ��������ɫ������
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//���ö�����ɫ��Դ��
	glCompileShader(vertexShader);//���붥����ɫ��Դ��
	//���glCompileShader�Ƿ����ɹ�
	int success;
	char infolog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//���Shader������
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);//��ñ��������Ϣ
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infolog << std::endl;
	}

	/*Ƭ����ɫ��(Fragment Shader) //�붥����ɫ����ͬС��*/
	unsigned int fragementShader;
	fragementShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragementShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragementShader);
	glGetShaderiv(fragementShader, GL_COMPILE_STATUS, &success);//���Shader������
	if (!success)
	{
		glGetShaderInfoLog(fragementShader, 512, NULL, infolog);//��ñ��������Ϣ
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infolog << std::endl;
	}
#pragma endregion

#pragma region ������ɫ��
	/*��ɫ������*/
	unsigned int shaderPragram;
	shaderPragram = glCreateProgram();//����һ����ɫ������������ID����
	glAttachShader(shaderPragram, vertexShader);//������ɫ��
	glAttachShader(shaderPragram, fragementShader);
	glLinkProgram(shaderPragram);//������ɫ��
	//�����ɫ�������Ƿ�ɹ�
	glGetProgramiv(shaderPragram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderPragram, 512, NULL, infolog);
		std::cout << "������ɫ��ʧ��\n" << infolog << std::endl;
	}
	glDeleteShader(vertexShader);//������ɺ�Ͳ���Ҫ��ɫ���� ɾ������
	glDeleteShader(fragementShader);
#pragma endregion

#pragma region ���ö������������������ͻ���/�����������
	float vertexs1[] = {//�����ε���������λ����������ɫ��Ϣ
	//����	  
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
	float vertexs2[] = {//�ı��εĶ�������
	//--λ��--        --��ɫ   --����uv����
	0.5f, 0.5f, 0.0f,  1,0,0,	1,1,	// ���Ͻ�
	0.5f, -0.5f,0.0f,  0,1,0,	1,0,	// ���½�
	-0.5f,-0.5f,0.0f,  0,0,1,	0,0	,// ���½�
	-0.5f, 0.5f,0.0f,  1,1,0,	0,1	// ���Ͻ�	
	};
	unsigned int indices[] = {//��2�������λ��Ƴ�һ���ı���(������0��ʼ)
		0, 1, 3, // ��һ��������
	1, 2, 3  // �ڶ���������
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);//��������������� �붥�㻺���������
	glGenBuffers(1, &VBO);//����һ�����㻺����� ����1Ϊ�ö����ID,ʱ��һ�޶���
	glGenBuffers(1, &EBO);//�����������
	glBindVertexArray(VAO);//�󶨶������ݶ���(�ڻ������֮ǰ��,��������ʹ�û������)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//OpenGL�кܶ໺��������ͣ����㻺�����Ļ���������GL_ARRAY_BUFFER��
									   //OpenGL��������ͬʱ�󶨶�����壬ֻҪ�����ǲ�ͬ�Ļ������͡�ʹ��glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ���ϣ�
	//�Ѷ������ݸ��Ƶ���������� �����ĵ�������ʽ:GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
												//GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
												//GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//��һ�������������,����Ͷ��㻺���������ʹ��,Ŀ���ǽ�Լ�ظ��Ķ������ݿռ�
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	/*���Ӷ�������*/
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
	glVertexAttribPointer������
	1.ָ��Ҫ���õĶ������� �������ɫ��Դ��������layout(location=0) ����������0��֮��Ӧ
	2.�������Դ�С ������һ��vec3,������ֵ,����������3
	3.������������ ������float��
	4.�¸��������������Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣���ǰ�������ΪGL_FALSE��
	5.��������,�������Ķ���������֮����� ÿ�����㶼��3��float,������������
	6.��ʾλ�������ڻ�������ʼλ�õ�ƫ����������λ������������Ŀ�ͷ������������0
	*/
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
#pragma endregion
	/*glUseProgram(shaderPragram);//����uniform��ֵ
	glUniform1f(glGetAttribLocation(shaderPragram, "offset"), 30);*/

#pragma region ���������ͼ
	//����texture����
	/*��һ����ͼ*/
	unsigned int texture1;
	glGenTextures(1, &texture1);//1.������������ 2.����洢id
	glBindTexture(GL_TEXTURE_2D, texture1);
	//Ϊ��ǰ�󶨵��������û��ơ����˷�ʽ
	//��ͼ��uv�������ǰ�str���ֵ�,��Ӧxyz.����������½ǿ�ʼ,������ֵΪ0-1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//ʹ��stb_image���ص�һ��ͼƬ
	int width1, height1, nChannels1;
	const char* fileName1 = "D:\\_LearOpenGL\\Project1\\Texture\\container.jpg";
	unsigned char* data1 = stbi_load(fileName1, &width1, &height1, &nChannels1, 0);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		/*����:
			1.����Ŀ��
			2.ָ���༶��Զ������,ϣ���ֶ�����ÿ���༶��Զ������,����Ϊ0,��Ϊ��������
			3.OpenGL������洢Ϊ���ָ�ʽ
			4.5.��Ⱥ͸߶�
			6.��������
			7.ԭͼ��ʽ
			8.��������.
			9.ͼ������
		*/
		glGenerateMipmap(GL_TEXTURE_2D);//�Զ�Ϊ��ǰ�󶨵��������ɶ༶��������
	}
	else {
		std::cout << "ͼƬ���ش���: " << std::endl;
	}
	stbi_image_free(data1);//�ͷ�����

	/*�ڶ�����ͼ*/
	unsigned int texture2;
	glGenTextures(1, &texture2);//1.������������ 2.����洢id
	glBindTexture(GL_TEXTURE_2D, texture2);
	//Ϊ��ǰ�󶨵��������û��ơ����˷�ʽ
	//��ͼ��uv�������ǰ�str���ֵ�,��Ӧxyz.����������½ǿ�ʼ,������ֵΪ0-1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//ʹ��stb_image���صڶ���ͼƬ
	int width2, height2, nChannels2;
	const char* fileName2 = "D:\\_LearOpenGL\\Project1\\Texture\\awesomeface.png";
	stbi_set_flip_vertically_on_load(true);//���غ�תͼƬy��
	unsigned char* data2 = stbi_load(fileName2, &width2, &height2, &nChannels2, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		/*����:
			1.����Ŀ��
			2.ָ���༶��Զ������,ϣ���ֶ�����ÿ���༶��Զ������,����Ϊ0,��Ϊ��������
			3.OpenGL������洢Ϊ���ָ�ʽ
			4.5.��Ⱥ͸߶�
			6.��������
			7.ԭͼ��ʽ
			8.��������.
			9.ͼ������
		*/
		glGenerateMipmap(GL_TEXTURE_2D);//�Զ�Ϊ��ǰ�󶨵��������ɶ༶��������
	}
	else {
		std::cout << "ͼƬ���ش���: " << std::endl;
	}
	stbi_image_free(data2);//�ͷ�����

	glUseProgram(shaderPragram);
	glUniform1i(glGetUniformLocation(shaderPragram, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderPragram, "texture2"), 1);
#pragma endregion

#pragma region ����仯��Ϊ��ά����
	/*����ģ�;���(�ֲ�����ת����������) ��Ұ����(��������ת�۲�����) ͸�Ӿ���(�۲�����ת�ü�����)*/
	glm::mat4 model = glm::mat4(1);
	//����д.0f Ч��:һ��ģ�;���,��x����ת
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1, 0, 0));
	glm::mat4 view = glm::mat4(1);
	//���������ƶ���������������ǰ�ƶ�(��Ϊ��������ӿڷ���Ϊ��������������-z)
	view = glm::translate(view, glm::vec3(0, 0, -3));
	glm::mat4 projection = glm::mat4(1);
	//͸��ͶӰ���� 1.��Ұ��С 2.��Ұ��߱� 3.��ƽ����� 4.Զƽ�����
	projection = glm::perspective(glm::radians(45.0f), 800 / 600.0f, 0.1f, 100.0f);
	/*���þ����������ɫ��*/
	glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glEnable(GL_DEPTH_TEST);//����z��Ȳ���(Ĭ��û����),֮���һֱ����,ֱ������glDisable
	//֮��Ҫ��glClear������ָ�������Ȼ��壨����ǰһ֡�������Ϣ��Ȼ�����ڻ����У�
	//glDisable(GL_DEPTH_TEST);//�ر���Ȳ���
	/*����ͼ�ε�λ����Ϣ*/
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

	/* //��ת/��������
	glm::mat4 trans = glm::mat4(1);
	trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0, 0, 1));
	trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
	glUseProgram(shaderPragram);
	glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
	*/
#pragma region ѭ����Ⱦ����
	/*ѭ����Ⱦ,�ڳ���ر�ǰһֱ����ͼ�� ���������þͻ������Ⱦһ�ξ�ֹͣ��Ⱦ��*/
	while (!glfwWindowShouldClose(window))
	{
		/*����*/
		processInput(window);
		/*��Ⱦָ��*/
		//����ϴ���Ⱦ��ͼ��
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//����������Ļ��ɫ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//������Ļ ��ֹ��ʾ��һ����Ⱦ����ĻЧ��
		//��Ⱦ������
		glActiveTexture(GL_TEXTURE0);//һ������Ϊ1������Ԫ,��ֻ����һ������ʱ,Ĭ������Ԫ���Ϊ0,���Զ������
		//OpenGL��16������Ԫ(0-15)
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
			glDrawArrays(GL_TRIANGLES, 0, 36);//ʹ�õĶ��㻺����������
		}
		glm::mat4 view = glm::mat4(1);
		//float radius = 10.0f;//�������������
		//float clamx = sin(glfwGetTime()) * radius;
		//float clamz = cos(glfwGetTime()) * radius;
		////����ָ��λ�õľ��� 1.����λ�� 2.Ŀ��λ�� 3.Ŀ���������
		//view = glm::lookAt(glm::vec3(clamx, 0, clamz), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//ʹ�õ�������������� 6:������Ŀ 0:ƫ����
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 projection = glm::mat4(1);
		projection = glm::perspective(glm::radians( camera.Zoom), SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderPragram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		/*��鲢�����¼� ��������*/
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

#pragma region ѭ����������
	glDeleteVertexArrays(1, &VAO);//������ɾ�������������
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate(); //�ͷ�/ɾ�������������Դ
#pragma endregion

	return 0;
}