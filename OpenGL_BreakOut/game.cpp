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
#include "physics.h"
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
PostProcessor* Effects;
TextRenderer* Text;
Physics* Physics_;

GLfloat ShakeTime = 0.0f;

void ActivatePowerUp(PowerUp& powerUp);
GLboolean isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);
GLboolean ShouldSpawn(GLuint chance);


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
	// Physics
	Physics_ = new Physics(this->Width, this->Height);

	// Text
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("resources/fonts/ocraext.TTF", 24);
	// BGM
	SoundEngine->play2D("resources/audio/breakout.mp3", GL_TRUE);

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
	ResourceManager::LoadTexture("resources/textures/cpdckppnlv.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("resources/textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("resources/textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("resources/textures/powerup_speed.png", GL_TRUE, "tex_speed");
	ResourceManager::LoadTexture("resources/textures/powerup_sticky.png", GL_TRUE, "tex_sticky");
	ResourceManager::LoadTexture("resources/textures/powerup_passthrough.png", GL_TRUE, "tex_pass");
	ResourceManager::LoadTexture("resources/textures/powerup_increase.png", GL_TRUE, "tex_size");
	ResourceManager::LoadTexture("resources/textures/powerup_confuse.png", GL_TRUE, "tex_confuse");
	ResourceManager::LoadTexture("resources/textures/powerup_chaos.png", GL_TRUE, "tex_chaos");

	// 加载关卡
	GameLevel one; one.Load("resources/levels/one.lvl", *Physics_, this->Width, this->Height * 0.5);
	GameLevel two; two.Load("resources/levels/two.lvl", *Physics_, this->Width, this->Height * 0.5);
	GameLevel three; three.Load("resources/levels/three.lvl", *Physics_, this->Width, this->Height * 0.5);
	GameLevel four; four.Load("resources/levels/four.lvl", *Physics_, this->Width, this->Height * 0.5);
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
	Player->Collision = Physics_->CreatePlayerPhysics(*Player);

	// 加载球
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
	Ball->Collision = Physics_->CreateBallPhysics(*Ball);

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
}

void Game::Update(GLfloat dt)
{
	Physics_->Update(dt);

	if (this->State == GameState::GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = GL_TRUE;
		this->State = GameState::GAME_WIN;
	}
	if (this->State == GameState::GAME_ACTIVE)
	{
		// object update
		Ball->Update(dt);
		for (auto& box : Levels[Level].Bricks) {
			box.Update(dt);
		}
		Player->Update(dt);

		Ball->Move(dt, this->Width);
		//DoCollisions();
		DoCollisionsBox2D();

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
		}
	}
	if (this->State == GameState::GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GameState::GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
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
	Physics_->Render();
	return;
	if (this->State == GameState::GAME_ACTIVE || this->State == GameState::GAME_MENU || this->State == GameState::GAME_WIN)
	{
		// 需要手动调整绘制顺序

		Effects->BeginRender();

		// 绘制背景
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
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
	if (this->State == GameState::GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", Width / 2 - 100.0f, Height / 2, 1.0f);
		Text->RenderText("Press W or S to select level", Width / 2 - 130.0f, Height / 2 + 20.0f, 0.75f);
	}
	if (this->State == GameState::GAME_WIN)
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

void Game::DoCollisionsBox2D() {

}

void Game::ResetLevel() {
	if (this->Level == 0)
		this->Levels[0].Load("resources/levels/one.lvl", *Physics_, this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("resources/levels/two.lvl", *Physics_, this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("resources/levels/three.lvl", *Physics_, this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("resources/levels/four.lvl", *Physics_, this->Width, this->Height * 0.5f);

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

GLboolean ShouldSpawn(GLuint chance)
{
	GLuint random = rand() % chance;
	return random == 0;
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