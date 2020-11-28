#include "game.h"
#include"BallObject.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include"ParticleGenerator.h"
#include"PostProcessor.h"
#include"PowerUp.h"
#include"TextRenderer.h"

#include<iostream>

#include<irrKlang-64bit-1.6.0/irrKlang.h>
using namespace irrklang;

/// <summary>
/// 碰撞检测返回的元组信息
///是否碰撞  碰撞来自哪个方向  碰撞进入物体的偏移量
/// </summary>
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

SpriteRenderer* Renderer;

//玩家（挡板)数据
const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY = 500.0f;
GameObject* Player;

//球数据
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const GLfloat BALL_RADIUS = 12.5f;
BallObject* Ball;

//粒子生成器
ParticleGenerator* particleGenerator;

//后处理
PostProcessor* postprocessor;
GLfloat shakeTime = 0;

//音效
ISoundEngine* soundEngine = createIrrKlangDevice();

//文本
TextRenderer* text;

#pragma region 道具相关

/// <summary>
/// 随机道具生成的概率
///概率为1/chance
/// </summary>
GLboolean ShouldSpawn(GLuint chance)
{
	return rand() % chance == 0;
}

void Game::SpawnPowerUps(glm::vec2 position)
{
	if (ShouldSpawn(5))
	{
		Texture2D texture = ResourceManager::GetTexture("powerup_speed");
		PowerUps.push_back(PowerUp("speed", glm::vec3(0.5, 0.5, 1), 0, position, texture));
	}
	if (ShouldSpawn(5))
	{
		Texture2D texture = ResourceManager::GetTexture("powerup_speed");
		PowerUps.push_back(PowerUp("speed", glm::vec3(0.5, 0.5, 1), 0, position, texture));
	}
	if (ShouldSpawn(5))
	{
		Texture2D texture = ResourceManager::GetTexture("powerup_sticky");
		PowerUps.push_back(PowerUp("sticky", glm::vec3(1, 0.5, 1), 20, position, texture));
	}
	if (ShouldSpawn(5))
	{
		Texture2D texture = ResourceManager::GetTexture("powerup_passthrough");
		PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5, 1, 0.5), 10, position, texture));
	}
	if (ShouldSpawn(5))
	{
		Texture2D texture = ResourceManager::GetTexture("powerup_increase");
		PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1, 0.6, 0.4), 0, position, texture));
	}
	if (ShouldSpawn(5))//负面道具概率高一点
	{
		Texture2D texture = ResourceManager::GetTexture("powerup_confuse");
		PowerUps.push_back(PowerUp("confuse", glm::vec3(1, 0.3, 0.3), 15, position, texture));
	}
	if (ShouldSpawn(5))
	{
		Texture2D texture = ResourceManager::GetTexture("powerup_chaos");
		PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9, 0.25, 0.25), 15, position, texture));
	}
}

GLboolean IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string& type)
{
	for (auto& p : powerUps)
	{
		if (p.Activated && p.Type == type)
			return true;
	}
	return false;
}

void Game::UpdatePowerUps(GLfloat dt)
{
	for (int i = 0; i < PowerUps.size(); i++)
	{
		PowerUps[i].Position += PowerUps[i].Velocity * dt;
		if (PowerUps[i].Activated)
		{
			PowerUps[i].Duration -= dt;
			if (PowerUps[i].Duration <= 0)
			{
				PowerUps[i].Activated = false;
				if (PowerUps[i].Type == "sticky")
				{
					if (!IsOtherPowerUpActive(PowerUps, PowerUps[i].Type))
					{
						Ball->Sticky = false;
						Player->Color = glm::vec3(1);
					}
				}
				else if (PowerUps[i].Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(PowerUps, PowerUps[i].Type))
					{
						Ball->PassThrough = false;
						Ball->Color = glm::vec3(1);
					}
				}
				else if (PowerUps[i].Type == "confuse")
				{
					if (!IsOtherPowerUpActive(PowerUps, PowerUps[i].Type))
						postprocessor->Confuse = false;
				}
				else if (PowerUps[i].Type == "chaos")
				{
					if (!IsOtherPowerUpActive(PowerUps, PowerUps[i].Type))
						postprocessor->chaos = false;
				}
				PowerUps.erase(PowerUps.begin() + i);
				i--;
			}
		}
	}
}

void ActivatePowerUp(PowerUp& powerUp)
{
	if (powerUp.Type == "speed")
		Ball->Velocity *= 1.2f;
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = true;
		Player->Color = glm::vec3(1, 0.5, 0.5);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = true;
		Player->Color = glm::vec3(1, 0.5, 0.5);
	}
	else if (powerUp.Type == "confuse")
	{
		if (postprocessor->Confuse == false)
			postprocessor->Confuse = true;
	}
	else if (powerUp.Type == "chaos")
	{
		if (postprocessor->chaos == false)
			postprocessor->chaos = true;
	}
}

#pragma endregion

Game::Game(GLuint width, GLuint height)
	: State(GAME_MENU), Keys(), Width(width), Height(height), Lives(3)
{

}

Game::~Game()
{
	delete Player;
	delete Renderer;
	delete particleGenerator;
	delete postprocessor;
	soundEngine->drop();
}

void Game::Init()
{
	// 加载shaders
	ResourceManager::LoadShader("spritever.txt", "spritefrag.txt", nullptr, "sprite");
	ResourceManager::LoadShader("particlever.txt", "particlefrag.txt", nullptr, "particle");
	ResourceManager::LoadShader("postprocessver.txt", "postprocessfrag.txt", nullptr, "postprocessing");
	ResourceManager::LoadShader("blockver.txt", "blockfrag.txt", nullptr, "block");

	// 加载图片
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/block.png", false, "block");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/particle.png", true, "particle");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/powerup_chaos.png", GL_TRUE, "powerup_chaos");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/powerup_confuse.png", GL_TRUE, "powerup_confuse");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/powerup_increase.png", GL_TRUE, "powerup_increase");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/powerup_speed.png", GL_TRUE, "powerup_speed");
	ResourceManager::LoadTexture("D:/_OpenGLDemo/Textures/powerup_sticky.png", GL_TRUE, "powerup_sticky");

	Shader shader = ResourceManager::GetShader("sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(Width), static_cast<GLfloat>(Height), 0.0f, -1.0f, 1.0f);
	shader.Use().SetMatrix4("projection", projection);
	shader.SetInteger("image", 0);
	Renderer = new SpriteRenderer(shader);

	Shader blockShader = ResourceManager::GetShader("block");
	blockShader.Use().SetInteger("image", 0);

	//加载关卡
	Texture2D blockSprite = ResourceManager::GetTexture("block");
	blockShader.SetMatrix4("projection", projection);
	GameLevel one(blockShader, blockSprite); one.Load("one.lvl", this->Width, this->Height * 0.5);
	GameLevel two(blockShader, blockSprite); two.Load("tow.lvl", Width, Height * 0.5);
	GameLevel three(blockShader, blockSprite); three.Load("three.lvl", Width, Height * 0.5);
	GameLevel four(blockShader, blockSprite); four.Load("four.lvl", Width, Height * 0.5);
	gameLevels.push_back(one);
	gameLevels.push_back(two);
	gameLevels.push_back(three);
	gameLevels.push_back(four);
	nowLevel = 0;

	//玩家
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y);
	Texture2D playerSp = ResourceManager::GetTexture("paddle");
	Player = new GameObject(playerPos, PLAYER_SIZE, playerSp);

	//球
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Texture2D faceTex = ResourceManager::GetTexture("face");
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, faceTex);

	//粒子生成器
	Shader particleShader = ResourceManager::GetShader("particle");
	particleShader.Use().SetInteger("sprite", 0);
	particleShader.SetMatrix4("projection", projection);
	Texture2D particleSprite = ResourceManager::GetTexture("particle");
	particleGenerator = new ParticleGenerator(particleShader, particleSprite, 500);

	//后处理
	Shader poseProcessShader = ResourceManager::GetShader("postprocessing");
	postprocessor = new PostProcessor(poseProcessShader, Width, Height);

	//audio
	soundEngine->setSoundVolume(0.1);
	soundEngine->play2D("D:/_OpenGLDemo/Audio/BGMusic.mp3", true);

	//文本
	text = new TextRenderer(Width, Height);
	text->Load("D:/_OpenGLDemo/Fonts/FZYTK.TTF", 24);
}

void Game::ProcessInput(GLfloat dt)
{
	if (State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		//移动玩家
		if (Keys[GLFW_KEY_A])
		{
			if (Player->Position.x > 0)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)//球如果没有发射出去就和玩家同步移动
					Ball->Position.x -= velocity;
			}
		}
		else if (Keys[GLFW_KEY_D])
		{
			if (Player->Position.x+Player->Size.x< Width)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}

		}
		//按下空格，发射出去球就开始游戏
		else if (Keys[GLFW_KEY_SPACE])
		{
			Ball->Stuck = false;
		}
	}

	else if (State == GAME_MENU)
	{
		if (Keys[GLFW_KEY_A]&&!KeysProcessed[GLFW_KEY_A])
		{
			if (nowLevel == 0)
				nowLevel = 3;
			else
				nowLevel--;
			KeysProcessed[GLFW_KEY_A] = true;
		}
		else if (Keys[GLFW_KEY_D]&&!KeysProcessed[GLFW_KEY_D])
		{
			nowLevel = (nowLevel + 1) % 4;
			KeysProcessed[GLFW_KEY_D] = true;
		}
		else if (Keys[GLFW_KEY_ENTER] && !KeysProcessed[GLFW_KEY_ENTER])
		{
			State = GAME_ACTIVE;
			KeysProcessed[GLFW_KEY_ENTER] = true;
		}
	}
}

void Game::Render()
{
	postprocessor->BeginRender();
	Texture2D background = ResourceManager::GetTexture("background");
	Renderer->DrawSprite(background, glm::vec2(0), glm::vec2(Width, Height), 0);
	gameLevels[nowLevel].Draw();
	Player->Draw(*Renderer);
	for (PowerUp& pow : PowerUps)
	{
		if (pow.IsDestroied == false)
			pow.Draw(*Renderer);
	}
	particleGenerator->Draw();
	Ball->Draw(*Renderer);
	text->RenderText("Lives" + std::to_string(Lives), glm::vec2(5), 1);
	postprocessor->EndRender();
	postprocessor->Render(glfwGetTime());

	//渲染菜单状态的提示文本
	if (State == GAME_MENU)
	{
		text->RenderText("Press ENTER to start", glm::vec2(250, Height / 2.0f), 1);
		text->RenderText("Press W or S to select level", glm::vec2(245, Height / 2.0f+20.0f), 0.75f);
	}
	else if (State == GAME_WIN)
	{
		text->RenderText(
			"You WON!!!",glm::vec2( 320.0, Height / 2 - 20.0), 1.0, glm::vec3(0.0, 1.0, 0.0)
		);
		text->RenderText(
			"Press ENTER to retry or ESC to quit",glm::vec2( 130.0, Height / 2), 1.0, glm::vec3(1.0, 1.0, 0.0)
		);

	}
}

//矩形与矩形碰撞检测
GLboolean CheckCollisionRectangle(GameObject& one, GameObject& tow)
{
	//x轴向检测
	bool collisionX = one.Position.x + one.Size.x >= tow.Position.x &&
		tow.Position.x + tow.Size.x >= one.Position.x;
	bool collisionY = one.Position.y + one.Size.y >= tow.Position.y &&
		tow.Position.y + tow.Size.y >= one.Position.y;
	return collisionX && collisionY;
}

/// <summary>
/// 计算球碰撞的方向
/// </summary>
Direction VectorDirection(glm::vec2 target)
{
	//定义四个单位方向向量
	glm::vec2 direction[] = {
		glm::vec2(0,1),//上
		glm::vec2(1,0),//右
		glm::vec2(0,-1),//下
		glm::vec2(-1,0)//左
	};

	int res = 0;
	target = glm::normalize(target);
	for (int i = 1; i < 3; i++)
	{
		if (glm::dot(target, direction[res]) > glm::dot(target, direction[i]))
			res = i;
	}
	return (Direction)res;
}

//圆形与矩形碰撞检测
Collision CheckCollisionCircle(BallObject& circle, GameObject& rect)
{
	//原地位于物体的左上方，所以需要再求一次中心点
	glm::vec2 circleCenter = circle.Position + circle.Size / 2.0f;
	glm::vec2 rectCenter = rect.Position + rect.Size / 2.0f;

	glm::vec2 difference = circleCenter - rectCenter;
	//获得一条从矩形中心点到最近点的向量
	glm::vec2 clamped = glm::clamp(difference, rect.Size / -2.0f, rect.Size / 2.0f);
	//用上面的向量加上中心的就得到了最近点的坐标
	glm::vec2 closest = clamped + rectCenter;

	difference = circleCenter - closest;
	GLfloat distance = glm::length(difference);

	//判断是否碰撞
	if (distance <= circle.Radius)
	{
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	}

	return std::make_tuple(GL_FALSE, UP, glm::vec2(0));
}

/// <summary>
/// 检测球/奖励道具是否与砖块碰撞
/// </summary>
void Game::DoCollisions()
{
	for (int i=0;i< gameLevels[nowLevel].amount;i++)
	{
		GameObject& go = gameLevels[nowLevel].Bricks[i];
		if (go.IsDestroied == false)
		{
			Collision collision = CheckCollisionCircle(*Ball, go);
			if (std::get<0>(collision))
			{
				if (go.IsSolid == false)//不是硬砖块就直接摧毁
				{
					SpawnPowerUps(go.Position);
					gameLevels[nowLevel].DestoryBlock(i);
					soundEngine->play2D("D:/_OpenGLDemo/Audio/Button.mp3", false);
				}

				//else//是硬砖块，产生屏幕抖动特性
				//{
				//	
				//}
				shakeTime = 0.5f;
				postprocessor->Shake = true;
				Direction direction = std::get<1>(collision);

				//当小球有PassThrough效果并且撞到的不是固体砖块我们就不发生碰撞
				if (!(Ball->PassThrough && go.IsSolid == false))
				{
					if (direction == RIGHT || direction == LEFT)//水平方向
					{
						//改变球体的速度
						Ball->Velocity.x = -Ball->Velocity.x;

						//重定位球体 球体和其他物体碰撞时可能已经进入物体内部了
						//通过重定位球体位置制作出只是碰到了物体的表面没有进入物体内部的效果
						//计算碰撞进入物体的向量
						//使用圆心到最近矩形点方向的半径向量减去圆心到最近点的向量(这里我们直接就获得其偏移长度)
						GLfloat offset = Ball->Radius - glm::abs(std::get<2>(collision).x);
						if (direction == RIGHT)
							Ball->Position.x += offset;
						else
							Ball->Position.x -= offset;
					}
					else if (direction == UP || direction == DOWN)
					{
						Ball->Velocity.y = -Ball->Velocity.y;

						GLfloat offset = Ball->Radius - glm::abs(std::get<2>(collision).y);
						if (direction == UP)
							Ball->Position.y -= offset;
						Ball->Position.y += offset;
					}
				}
			}
		}
	}

	for (PowerUp& powerUp : PowerUps)
	{
		if (powerUp.IsDestroied == false)
		{
			if (powerUp.Position.y >= Height)
				powerUp.IsDestroied = true;
			else
			{
				GLboolean res = CheckCollisionRectangle(*Player, powerUp);
				if (res)
				{
					ActivatePowerUp(powerUp);
					powerUp.IsDestroied = true;
					powerUp.Activated = true;
				}
			}
		}
	}
}

void Game::Reset()
{
	//重置挡板位置
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y);
	Player->Position = playerPos;

	//重载球位置
	glm::vec2 ballPos= playerPos + glm::vec2(Player->Size.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball->Reset(ballPos, INITIAL_BALL_VELOCITY);
}

void Game::Update(GLfloat dt)
{
	DoCollisions();
	Ball->Move(dt, Width);

	//检查球和挡板是否碰撞
	if (Ball->Stuck == false)
	{
		Collision collision = CheckCollisionCircle(*Ball, *Player);
		if (std::get<0>(collision))
		{
			Ball->Stuck = Ball->Sticky;
			Ball->Sticky = false;
			GLfloat centernBorderX = Player->Position.x + Player->Size.x / 2.0f;
			GLfloat centerCircleX = Ball->Position.x + Ball->Size.x / 2.0f;
			GLfloat distance = glm::abs(centerCircleX - centernBorderX);

			GLfloat percent = Player->Size.x / 2.0f / distance;
			GLfloat strength = 2;

			glm::vec2 oldVel = Ball->Velocity;
			Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percent * strength;

			Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVel);
			Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
		}
	}

	particleGenerator->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));

	UpdatePowerUps(dt);

	//监听屏幕抖动后处理
	if (shakeTime > 0)
	{
		shakeTime -= dt;
		if (shakeTime <= 0)
			postprocessor->Shake = false;
	}

	//检查游戏是否结束
	if (State==GAME_ACTIVE&& gameLevels[nowLevel].IsCompleted())
	{
		State = GAME_WIN;
	}

	//检查小球是否出界
	if (Ball->Position.y >= Height)
	{
		std::cout << "出街\n";
		Lives--;
		Reset();
		if (Lives <= 0)
		{
			std::cout << "游戏结束\n";
		}

	}

}