/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_H
#define GAME_H
#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件
#include <vector>
#include "game_level.h"
#include "power_up.h"


// Represents the current state of the game
enum class GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// 初始化挡板的大小
const glm::vec2 PLAYER_SIZE(100, 20);
// 初始化当班的速率
const GLfloat PLAYER_VELOCITY(500.0f);
// 初始化球的速度
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// 球的半径
const GLfloat BALL_RADIUS = 12.5f;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // Game state
    GameState              State;
    GLboolean              Keys[1024];
    GLboolean              KeysProcessed[1024];
    GLuint                 Width, Height;
    std::vector<GameLevel> Levels;
    GLuint                 Level;
    std::vector<PowerUp>  PowerUps;
    GLuint Lives;

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
    void ResetLevel();
    void ResetPlayer();
    void SpawnPowerUps(GameObject& block);
    void UpdatePowerUps(GLfloat dt);
};

#endif