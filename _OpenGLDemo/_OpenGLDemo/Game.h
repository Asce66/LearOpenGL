
#ifndef GAME_H
#define GAME_H

#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include"GameLevel.h"
#include"PowerUp.h"

#include<vector>

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum Direction {
    UP=0,
    RIGHT,
    DOWN,
    LEFT
};

class Game
{
public:
    // Game state
    GameState              State;
    GLboolean              Keys[1024];
    GLboolean              KeysProcessed[1024];//已经处理过的按键，
    GLuint                 Width, Height;//防止按下一个按键后多帧都检测到按下，因而处理多次的错误
    GLuint                  Lives;

    std::vector<GameLevel>gameLevels;
    GLuint nowLevel;

    //道具
    std::vector<PowerUp>PowerUps;
    void SpawnPowerUps(glm::vec2 position);
    void UpdatePowerUps(GLfloat dt);

    // Constructor/Destructor
    Game(GLuint width, GLuint height);
    ~Game();
    // Initialize game state (load all shaders/textures/levels)
    void Init();
    // GameLoop
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    void DoCollisions();
    void Reset();
};

#endif


