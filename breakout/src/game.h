#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "game_level.h"

// 代表了游戏的当前状态
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
}; 

class Game
{
    public:
        // 游戏状态
        GameState  State;   
        GLboolean  Keys[1024];
        std::vector<GameLevel> Levels;
        GLuint Level;
        GLuint     Width, Height;
        // 构造函数/析构函数
        Game(GLuint width, GLuint height);
        ~Game();
        // 初始化游戏状态（加载所有的着色器/纹理/关卡）
        void Init();
        // 游戏循环
        void ProcessInput(GLfloat dt);
        void Update(GLfloat dt);
        void Render();
        void DoCollisions();
        void ResetLevel();
        void ResetPlayer();

        std::vector<PowerUp>  PowerUps;
        void SpawnPowerUps(GameObject &block);
        void UpdatePowerUps(GLfloat dt);
};

#endif
