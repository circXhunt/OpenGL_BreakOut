/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game.h"
#include "sprite_renderer.h"
#include <chrono>
#include <thread>
#include <functional>
#include "triangle_renderer.h"
#include "resource_manager.h"
#include <GLFW\glfw3.h>
#include "ball_object.h"
#include "Collision.h"
#include "particle_generator.h"
#include "post_processor.h"
#include <algorithm>
#include "debugger.h"
#include "irrKlang/irrKlang.h"
#include "texture_renderer.h"
#include "task.h"

using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
PostProcessor* Effects;
TextRenderer* Text;
GameObject* qb;

GLfloat ShakeTime = 0.0f;
GLboolean GameClear = GL_FALSE;
GLboolean GameSaved = GL_FALSE;
GameState LastGameState = GameState::GAME_MENU;
std::vector<Magia::Task> task_list;
Magia::Task menu_aduio_task, active_audio_task;

constexpr glm::vec3 SELECTED_COLOR = glm::vec3(1.0, 1.0, 0.0);
constexpr glm::vec3 UNSELECTED_COLOR = glm::vec3(1.0, 1.0, 1.0);
constexpr glm::vec3 UNACTIVE_COLOR = glm::vec3(0.7f, 0.7f, 0.7f);
glm::vec3 game_start_color = SELECTED_COLOR;
glm::vec3 game_continue_color = UNSELECTED_COLOR;
GLuint select_button = 0;

void ActivatePowerUp(PowerUp& powerUp);
GLboolean isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);
GLboolean ShouldSpawn(GLuint chance);

void Menu_Enter();
void Menu_Exit();
void Game_Enter();
void Game_Exit();
void Win_Enter();
void Win_Exit();

GLfloat fadeTime = 0.0f;
GLfloat temp_fadeTime = 0.0f;
GLfloat soundVol = 0.0f;
GLfloat temp_soundVol = 0.0f;
GLfloat soundVolMinus_perSecond = 0.0f;
void UpdateSound(GLfloat dt);
void SwitchToGameActive(Game* game);
std::function<void()> sound_callback;


Game::Game(GLuint width, GLuint height)
	:Level(0), State(GameState::GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Lives(3)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
}

void Game::Init()
{
	// Text
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("resources/fonts/ocraext.TTF", 24);
	// BGM
	SoundEngine->play2D("resources/audio/Sis puella magica!.mp3", GL_TRUE);

	// 加载着色器
	ResourceManager::LoadShader("resources/shaders/sprite.vs", "resources/shaders/sprite.fs", "sprite");
	ResourceManager::LoadShader("resources/shaders/particle.vs", "resources/shaders/particle.fs", "particle");
	ResourceManager::LoadShader("resources/shaders/effect.vs", "resources/shaders/effect.fs", "effect");

	// 配置着色器
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("sprite").use();
	ResourceManager::GetShader("sprite").setInt("image", 0);
	ResourceManager::GetShader("sprite").setMat4("projection", projection);

	ResourceManager::GetShader("particle").use();
	ResourceManager::GetShader("particle").setInt("sprite", 0);
	ResourceManager::GetShader("particle").setMat4("projection", projection);


	// 加载纹理
	ResourceManager::LoadTexture("resources/textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("resources/textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("resources/textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("resources/textures/300px-QB.jpg", GL_FALSE, "qb");
	ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("resources/textures/powerup_speed.png", GL_TRUE, "tex_speed");
	ResourceManager::LoadTexture("resources/textures/powerup_sticky.png", GL_TRUE, "tex_sticky");
	ResourceManager::LoadTexture("resources/textures/powerup_passthrough.png", GL_TRUE, "tex_pass");
	ResourceManager::LoadTexture("resources/textures/powerup_increase.png", GL_TRUE, "tex_size");
	ResourceManager::LoadTexture("resources/textures/powerup_confuse.png", GL_TRUE, "tex_confuse");
	ResourceManager::LoadTexture("resources/textures/powerup_chaos.png", GL_TRUE, "tex_chaos");
	ResourceManager::LoadTexture("resources/textures/background/menu.png", GL_TRUE, "menu");
	ResourceManager::LoadTexture("resources/textures/background/logo.png", GL_TRUE, "logo");


	// 加载关卡
	GameLevel one("one"); one.Load("resources/levels/one.lvl", this->Width, this->Height * 0.5);
	GameLevel two("two"); two.Load("resources/levels/two.lvl", this->Width, this->Height * 0.5);
	GameLevel three("three"); three.Load("resources/levels/three.lvl", this->Width, this->Height * 0.5);
	GameLevel four("four"); four.Load("resources/levels/four.lvl", this->Width, this->Height * 0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);

	this->Level = 0;

	// 加载玩家
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	// 加载球
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

	// 渲染器
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	//粒子
	Particles = new ParticleGenerator(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		500
	);

	// 后处理特效
	Effects = new PostProcessor(ResourceManager::GetShader("effect"), this->Width, this->Height);

	// GUI
	qb = new GameObject(glm::vec2(0.0f), glm::vec2(20, 20), ResourceManager::GetTexture("qb"));

	// task
	
}

void Game::Update(GLfloat dt)
{
	UpdateSound(dt);
	if (this->State != LastGameState)
	{
		switch (LastGameState)
		{
		case GameState::GAME_ACTIVE:
			Game_Exit();
			break;
		case GameState::GAME_MENU:
			Menu_Exit();
			break;
		case GameState::GAME_WIN:
			Win_Exit();
			break;
		default:
			break;
		}

		switch (this->State)
		{
		case GameState::GAME_ACTIVE:
			Game_Enter();
			break;
		case GameState::GAME_MENU:
			Menu_Enter();
			break;
		case GameState::GAME_WIN:
			Win_Enter();
			break;
		default:
			break;
		}
		LastGameState = this->State;
	}
	if (this->State == GameState::GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
	{
		this->NextLevel();
		//this->ResetLevel();
		//this->ResetPlayer();
		//Effects->Chaos = GL_TRUE;
		//this->State = GameState::GAME_WIN;
	}
	if (this->State == GameState::GAME_ACTIVE)
	{
		Ball->Move(dt, this->Width);

		DoCollisions();

		Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));

		this->UpdatePowerUps(dt);

		if (ShakeTime > 0.0f)
		{
			ShakeTime -= dt;
			if (ShakeTime <= 0.0f)
				Effects->Shake = false;
		}
		if (Ball->Position.y >= this->Height) // 球是否接触到底部边界?
		{
			--this->Lives;
			// 玩家是否已失去所有生命值? : 游戏结束
			if (this->Lives == 0)
			{
				this->ResetLevel();
				this->State = GameState::GAME_MENU;
			}
			this->ResetPlayer();
		}
	}

}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GameState::GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// 移动挡板
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0) {
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x) {
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;

		// Debug
		if (this->Keys[GLFW_KEY_X] && !this->KeysProcessed[GLFW_KEY_X])
		{
			for (auto& b : Levels[Level].Bricks) {
				if (!b.IsSolid)
				{
					b.Destroyed = GL_TRUE;
				}
			}
			this->KeysProcessed[GLFW_KEY_X] = GL_TRUE;
		}
	}
	if (this->State == GameState::GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_UP] && !this->KeysProcessed[GLFW_KEY_UP])
		{
			if (select_button > 0)
			{
				select_button--;
				SoundEngine->play2D("resources/audio/menu/select_move.mp3");
			}
			this->KeysProcessed[GLFW_KEY_UP] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_DOWN] && !this->KeysProcessed[GLFW_KEY_DOWN])
		{
			if (select_button == 0)
			{
				SoundEngine->play2D("resources/audio/menu/select_move.mp3");
				select_button++;
			}
			this->KeysProcessed[GLFW_KEY_DOWN] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			if (select_button == 0)
			{
				SoundEngine->play2D("resources/audio/menu/decision.mp3");
				SwitchState(GameState::GAME_ACTIVE);
			}

			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
	}
	if (this->State == GameState::GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			Effects->Chaos = GL_FALSE;
			this->State = GameState::GAME_MENU;
		}
	}

}

void Game::Render()
{
	if (this->State == GameState::GAME_MENU)
	{
		// 绘制背景
		Renderer->DrawSprite(ResourceManager::GetTexture("menu"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);

		// 绘制Logo
		Renderer->DrawSprite(ResourceManager::GetTexture("logo"),
			glm::vec2(this->Width / 2 - 260, this->Height / 2 - 270), glm::vec2(800, 200), 0.0f
		);
		glm::vec2 qbPos = glm::vec2(0.0f);
		if (select_button == 0)
		{
			qbPos = glm::vec2(this->Width / 2 - 41, Height / 2);
			game_start_color = SELECTED_COLOR;
			if (GameSaved)
			{
				game_continue_color = UNSELECTED_COLOR;
			}
			else
			{
				game_continue_color = UNACTIVE_COLOR;
			}
		}
		if (select_button == 1)
		{
			qbPos = glm::vec2(this->Width / 2 - 41, Height / 2 + 40);
			game_start_color = UNSELECTED_COLOR;
			if (GameSaved)
			{
				game_continue_color = UNSELECTED_COLOR;
			}
			else
			{
				game_continue_color = UNACTIVE_COLOR;
			}
		}
		Text->RenderText(
			"Game Start", this->Width / 2 - 11, Height / 2, 1.0, game_start_color
		);
		Text->RenderText(
			"Game Continue", this->Width / 2 - 13, Height / 2 + 40, 1.0, game_continue_color
		);
		qb->Position = qbPos;
		qb->Draw(*Renderer);
		if (GameClear)
		{
			Text->RenderText(
				"Level Select", 320.0, Height / 2 + 20, 1.0, glm::vec3(1.0, 1.0, 0.0)
			);
		}
	}
	else if (this->State == GameState::GAME_ACTIVE)
	{
		// 需要手动调整绘制顺序

		Effects->BeginRender();

		// 绘制背景
		Renderer->DrawSprite(this->Levels[this->Level].GetBackground(),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);

		// 绘制关卡
		this->Levels[this->Level].Draw(*Renderer);

		// Draw particles   
		Particles->Draw();

		// Draw Power up
		for (PowerUp& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);

		// 绘制玩家
		Player->Draw(*Renderer);

		// 绘制球
		Ball->Draw(*Renderer);



		Effects->EndRender();

		Effects->Render(glfwGetTime());

		// HUD
		std::stringstream ss; ss << this->Lives;

		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	else if (this->State == GameState::GAME_WIN)
	{
		Text->RenderText(
			"You WON!!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
		);
		Text->RenderText(
			"Press ENTER to retry or ESC to quit", 130.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
		);
	}
}

void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			auto result = Collision::CheckCollision(*Ball, box);
			if (std::get<0>(result))
			{
				if (!box.IsSolid) {

					box.Destroyed = GL_TRUE;
					this->SpawnPowerUps(box);
					SoundEngine->play2D("resources/audio/bleep.mp3", GL_FALSE);
				}
				else
				{   // 如果是实心的砖块则激活shake特效
					ShakeTime = 0.05f;
					Effects->Shake = true;
					SoundEngine->play2D("resources/audio/solid.wav", GL_FALSE);
				}
				Direction dir = std::get<1>(result);
				glm::vec2 diff_vector = std::get<2>(result);
				if (!(Ball->PassThrough && !box.IsSolid))
				{
					if (dir == LEFT || dir == RIGHT)
					{
						Ball->Velocity.x = -Ball->Velocity.x; // 反转水平速度
						// 重定位
						GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							Ball->Position.x += penetration; // 将球右移
						else
							Ball->Position.x -= penetration; // 将球左移
					}
					else // 垂直方向碰撞
					{
						Ball->Velocity.y = -Ball->Velocity.y; // 反转垂直速度
						// 重定位
						GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
							Ball->Position.y -= penetration; // 将球上移
						else
							Ball->Position.y += penetration; // 将球下移
					}
				}
			}
		}
	}
	auto result = Collision::CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		SoundEngine->play2D("resources/audio/bleep.wav", GL_FALSE);
		// 检查碰到了挡板的哪个位置，并根据碰到哪个位置来改变速度
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);
		// 依据结果移动
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);

		// If Sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
		Ball->Stuck = Ball->Sticky;
	}

	for (PowerUp& powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = GL_TRUE;
			if (Collision::CheckCollision(*Player, powerUp))
			{   // 道具与挡板接触，激活它！
				ActivatePowerUp(powerUp);
				SoundEngine->play2D("resources/audio/powerup.wav", GL_FALSE);
				powerUp.Destroyed = GL_TRUE;
				powerUp.Activated = GL_TRUE;
			}
		}
	}
}

void Game::ResetLevel() {
	if (this->Level == 0)
		this->Levels[0].Load("resources/levels/one.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("resources/levels/two.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("resources/levels/three.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("resources/levels/four.lvl", this->Width, this->Height * 0.5f);

	this->Lives = 3;
}

void Game::ResetPlayer() {
	// Reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
	// Also disable all active powerups
	for (PowerUp& powerUp : this->PowerUps) {
		powerUp.Destroyed = true;
	}
	this->PowerUps.clear();
	Effects->Chaos = Effects->Confuse = GL_FALSE;
	Ball->PassThrough = Ball->Sticky = GL_FALSE;
	Player->Color = glm::vec3(1.0f);
	Ball->Color = glm::vec3(1.0f);
}

void Game::NextLevel()
{	
	++Level;
	if (Level < Levels.size())
	{
		ResetLevel();
		ResetPlayer();
	}
	else
	{
		this->State = GameState::GAME_WIN;
	}
}

GLboolean ShouldSpawn(GLuint chance)
{
	GLuint random = rand() % chance;
	return random == 0;
}

void Menu_Enter()
{
	SoundEngine->play2D("resources/audio/Sis puella magica!.mp3", GL_TRUE);
}

void Menu_Exit()
{
	SoundEngine->stopAllSounds();
}

void Game_Enter()
{
	SoundEngine->play2D("resources/audio/breakout.mp3", GL_TRUE);
	fadeTime = 2.0f;
	soundVol = 1.0f;
	sound_callback = nullptr;
}

void Game_Exit()
{
	SoundEngine->stopAllSounds();
}

void Win_Enter()
{
}

void Win_Exit()
{
	SoundEngine->stopAllSounds();
}

void Game::SwitchState(GameState targetState) {

	if (targetState == GameState::GAME_ACTIVE)
	{
		if (this->State == GameState::GAME_MENU)
		{
			//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			//this->State = GameState::GAME_ACTIVE;
			fadeTime = 2.0f;
			soundVol = 0.0f;
			sound_callback = std::bind(&SwitchToGameActive, this);
		}
	}

}

void Game::SpawnPowerUps(GameObject& block)
{
	if (ShouldSpawn(75)) // 1/75的几率
		this->PowerUps.push_back(
			PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("tex_speed")
			));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("tex_sticky")
			));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("tex_pass")
			));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("tex_size")
			));
	if (ShouldSpawn(15)) // 负面道具被更频繁地生成
		this->PowerUps.push_back(
			PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("tex_confuse")
			));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(
			PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("tex_chaos")
			));
}

void Game::UpdatePowerUps(GLfloat dt)
{
	for (PowerUp& powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				// 之后会将这个道具移除
				powerUp.Activated = GL_FALSE;
				// 停用效果
				if (powerUp.Type == "sticky")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "sticky"))
					{   // 仅当没有其他sticky效果处于激活状态时重置，以下同理
						Ball->Sticky = GL_FALSE;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "pass-through"))
					{
						Ball->PassThrough = GL_FALSE;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "confuse"))
					{
						Effects->Confuse = GL_FALSE;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
					{
						Effects->Chaos = GL_FALSE;
					}
				}
			}
		}
	}
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

void ActivatePowerUp(PowerUp& powerUp)
{
	// 根据道具类型发动道具
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = GL_TRUE;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = GL_TRUE;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = GL_TRUE; // 只在chaos未激活时生效，chaos同理
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = GL_TRUE;
	}
}

GLboolean isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
	for (const PowerUp& powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return GL_TRUE;
	}
	return GL_FALSE;
}

void UpdateSound(GLfloat dt) {
	if (fadeTime != 0.0f)
	{
		temp_fadeTime = fadeTime;
		temp_soundVol = SoundEngine->getSoundVolume();
		soundVolMinus_perSecond = (soundVol - SoundEngine->getSoundVolume()) / fadeTime;
		//std::cout << "init vol:" << temp_soundVol << " per minus:" << soundVolMinus_perSecond << " " <<dt;

		fadeTime = 0.0f;
		soundVol = 0.0f;
	}
	if ((temp_fadeTime -= dt) < 0.0f)
	{
		if (sound_callback != nullptr)
		{
			sound_callback();
		}
	}
	else
	{
		temp_soundVol += (dt * soundVolMinus_perSecond);
		SoundEngine->setSoundVolume(temp_soundVol);
	}

}

void SwitchToGameActive(Game* game) {
	game->State = GameState::GAME_ACTIVE;
}