#include"model.h"
#include"../HelloTrangle/CAMER_H.h"

#include<ctime>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* , int , int );
void processInput(GLFWwindow* );
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scrollview_callback(GLFWwindow* window, double xOffset, double yOffset);

//相机
Camera camera(glm::vec3(0,0,3.0f));
float lastX=SCREEN_WIDTH/2.0f, lastY = SCREEN_HEIGHT/2.0f;
bool isFirstMouse = true;

//时间
float lastFrame = 0, deltaTime = 0;

int main() {

#pragma region 初始化窗口

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHi(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL_LoadModel", NULL, NULL);
	if (!window) {
		std::cout << "窗口初始化失败\n";
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scrollview_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "glad初始化失败\n";
		glfwTerminate();
		return 0;
	}

	glEnable(GL_DEPTH_TEST);

#pragma endregion

	stbi_set_flip_vertically_on_load(true);

	Shader rockShader("rockver.txt", "rockfrag.txt");

	std::string rockPath = "D:/_LearOpenGL/Project1/Model/rock/rock.obj";
	Model rock(rockPath);

	Shader shader("ver.txt", "frag.txt","geo.txt");//几何着色器实现爆破效果
	//Shader shader("vertNormal.txt","fragNormal.txt","geoNormal.txt");//几何著着色器实现法线可视乎
	std::string path = "D:/_LearOpenGL/Project1/Model/nanosuit/nanosuit.obj";
	Model robotModel(path);

	//设置每一个岩石的model矩阵
	const int amount = 100;
	glm::mat4 *modelMatrices = new glm::mat4[amount] ;
	float radius = 15.0f, offset =8;//环绕半径与小行星之间偏移量
	srand(time(0));
	for (int i = 0; i < amount; i++)
	{
		glm::mat4 model=glm::mat4(1);
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = cos(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = sin(angle) * radius + displacement;
		model = glm::translate(model,glm::vec3(x, y, z));

		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(1));

		float roatAngle = rand() % 360;
		model = glm::rotate(model, roatAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		modelMatrices[i] = model;
	}

	//链接岩石的model矩阵属性
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < rock.meshes.size(); i++)
	{
		unsigned int VAO = rock.meshes[i].GetVAO();
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)sizeof(glm::vec4));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2* sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3* sizeof(glm::vec4)));

		//实例化多少个物体后读取下一行数据
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindVertexArray(0);
	}
		
	while (!glfwWindowShouldClose(window)) {

		//计算帧率
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		processInput(window);

		glClearColor(0.5, 0.5, 0.5, 0.5);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		/*  法线可视化步骤****
			//使用正常着色器着色一次
			//使用法线可视化着色器再着色一次
		*/
		glm::mat4 view = camera.GetViewMatrix();			
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 1000.0f);
		rockShader.use();
		rockShader.setMat4("view", view);
		rockShader.setMat4("projection", projection);
		
		shader.use();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);			

		////模型渲染
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));

		shader.setMat4("model", model);
		shader.setFloat("time", glfwGetTime());
		robotModel.Draw(shader);

		rockShader.use();
		rockShader.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.texture_loaded.begin()->second.id);
		rockShader.setMat4("model", glm::mat4(1));
		/*rock.Draw(rockShader);
		for (int i = 0; i <3; i++)
		{
			rockShader.setMat4("model", modelMatrices[i]);
			rock.Draw(rockShader);
		}*/
		//渲染岩石
		for (int i = 0; i < rock.meshes.size(); i++)
		{
			unsigned int VAO = rock.meshes[i].GetVAO();
			glBindVertexArray(VAO);
			glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int heigth)
{
	glViewport(0, 0, width, heigth);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_S))
		camera.ProcessKeyBoard(BACKWORD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_W))
		camera.ProcessKeyBoard(FORWARD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_A))
		camera.ProcessKeyBoard(LEFT, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_D))
		camera.ProcessKeyBoard(RIGHT, deltaTime);

}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (isFirstMouse) {
		lastX = xPos;
		lastY = yPos;
		isFirstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scrollview_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScorll(yOffset);
}
