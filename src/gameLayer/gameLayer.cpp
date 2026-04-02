#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include <vector>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <glui/glui.h>
#include <platformTools.h>
#include <raudio.h>

#include <tileRenderer.h>
#include <bullet.h>
#include <enemy.h>
#include <item.h>


struct GamePlayData {
	glm::vec2 playerPos = { 100,100 };
	//glm::vec2 playerPrevPos = {};
	//glm::vec2 playerVelocity = {};

	std::vector<Bullet> bullets;
	std::vector<Enemy> enemies;
	std::vector<Item> items;

	float playerSpeed = 300.f;        // ↑ more responsive movement
	float playerMaxHealth = 100.f;
	float playerHealth = playerMaxHealth;
	float playerArmour = 50.f;
	float playerDamage = 65.f;       // ↑ enemies die faster
	float playerHealthRegen = 0.5f; // ↓ regen (less forgiving)
	float playerBulletSpeed = 900.f;  // ↑ snappy shooting

	float spawnEnemyTimerSeconds = 3.f;
};

GamePlayData data;

gl2d::Renderer2D renderer;

constexpr int BACKGROUNDS = 3;

gl2d::Texture healthBarTexture;
gl2d::Texture healthTexture;

gl2d::Texture spaceShipTexture;
gl2d::TextureAtlasPadding spaceShipAtlas;

gl2d::Texture bulletTexture;
gl2d::TextureAtlasPadding bulletAtlas;

gl2d::Texture backgroudTexture[BACKGROUNDS];
TileRenderer tileRenderer[BACKGROUNDS];

gl2d::Texture hpItemTexture;
gl2d::Texture armourItemTexture;

Sound shootSound;

std::unordered_map<ItemTypes, gl2d::Texture> itemTypeToTexture = {};


#pragma region functions

bool intersectBullet(glm::vec2 bulletPos, glm::vec2 shipPos, float shipSize)
{
	return glm::distance(bulletPos, shipPos) <= shipSize;
}

void restartGame()
{
	data = {};
	renderer.currentCamera.follow(data.playerPos, 550, 0, 0, renderer.windowW, renderer.windowH, 0.f);
}

void spawnEnemy()
{
	glm::uvec2 shipTypes[] = { {0,0},{0,1}, {2,0},{3,1} };

	Enemy e;
	e.position = data.playerPos;

	glm::vec2 offset(2000, 0);
	offset = glm::vec2(glm::vec4(offset, 0, 1) * 
		glm::rotate(glm::mat4(1.f), glm::radians((float)(rand() % 360)), glm::vec3(0, 0, 1)));

	e.position += offset;
	e.type = shipTypes[rand() % 4];

	//e.speed = e.speed + rand() % 100;
	//e.turnSpeed = 2.2f + (rand() & 1000) / 500.f;
	//e.fireRange = 700.f + (rand() % 1000) / 2.f;
	//e.fireThreashold = 0.8f;
	//e.fireTimeReset = 0.1 + (rand() % 1000) / 2000.f;
	//e.bulletSpeed = rand() % 3000 + 1000.f;

	data.enemies.push_back(e);
}

#pragma endregion

bool initGame()
{

#pragma region lib init

	//initializing stuff for the renderer
	gl2d::init();

	//initializing audio
	InitAudioDevice();

#pragma endregion


#pragma region renderer

	renderer.create();

#pragma endregion


#pragma region textures loading

	spaceShipTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "spaceShip/stitchedFiles/spaceships.png", 128, true);
	spaceShipAtlas = gl2d::TextureAtlasPadding(5, 2, spaceShipTexture.GetSize().x, spaceShipTexture.GetSize().y);
	 
	bulletTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "spaceShip/stitchedFiles/projectiles.png", 500, true);
	bulletAtlas = gl2d::TextureAtlasPadding(3, 2, bulletTexture.GetSize().x, bulletTexture.GetSize().y);

	healthBarTexture.loadFromFile(RESOURCES_PATH "healthBar.png", true);
	healthTexture.loadFromFile(RESOURCES_PATH "health.png", true);

	hpItemTexture.loadFromFile(RESOURCES_PATH "items/hp.png", true);
	armourItemTexture.loadFromFile(RESOURCES_PATH "items/armour.png", true);

	shootSound = LoadSound(RESOURCES_PATH "shoot.flac");
	SetSoundVolume(shootSound, 0.3f);

	backgroudTexture[0].loadFromFile(RESOURCES_PATH "background1.png", true);
	backgroudTexture[1].loadFromFile(RESOURCES_PATH "background2.png", true);
	backgroudTexture[2].loadFromFile(RESOURCES_PATH "background3.png", true);
	//backgroudTexture[3].loadFromFile(RESOURCES_PATH "background4.png", true);


	tileRenderer[0].texture = backgroudTexture[0];
	tileRenderer[1].texture = backgroudTexture[1];
	tileRenderer[2].texture = backgroudTexture[2];
	//tileRenderer[3].texture = backgroudTexture[3];

	tileRenderer[0].paralaxStrength = 0;
	tileRenderer[1].paralaxStrength = 0.5;
	tileRenderer[2].paralaxStrength = 0.7;
	//tileRenderer[3].paralaxStrength = 0.7;

#pragma endregion


#pragma region item type to texture map

	itemTypeToTexture[ItemTypes::health] = hpItemTexture;
	itemTypeToTexture[ItemTypes::armour] = armourItemTexture;


#pragma endregion


	restartGame();
	
	return true;
}


bool gameLogic(float deltaTime)
{

#pragma region init stuff

	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);

#pragma endregion


#pragma region player movement

	glm::vec2 move = {};

	if (
		platform::isButtonHeld(platform::Button::W) ||
		platform::isButtonHeld(platform::Button::Up)
		)
	{
		move.y = -1;
	}

	if (
		platform::isButtonHeld(platform::Button::S) ||
		platform::isButtonHeld(platform::Button::Down)
		)
	{
		move.y = 1;
	}

	if (
		platform::isButtonHeld(platform::Button::A) ||
		platform::isButtonHeld(platform::Button::Left)
		)
	{
		move.x = -1;
	}

	if (
		platform::isButtonHeld(platform::Button::D) ||
		platform::isButtonHeld(platform::Button::Right)
		)
	{
		move.x = 1;
	}

	if (move.x != 0 || move.y != 0)
	{
		move = glm::normalize(move);

		move *= deltaTime * data.playerSpeed;
		data.playerPos += move;
	}


#pragma endregion


#pragma region follow

	renderer.currentCamera.follow(data.playerPos, deltaTime * 450, 10, 50, w, h, deltaTime);

#pragma endregion


#pragma region render background

	renderer.currentCamera.zoom = 1;

	for (int i = 0; i < BACKGROUNDS; i++)
	{
		tileRenderer[i].render(renderer);
	}

#pragma endregion


#pragma region mouse pos

	glm::vec2 mousePos = platform::getRelMousePosition();
	glm::vec2 screenCenter(w / 2.f, h / 2.f);

	glm::vec2 mouseDirection = mousePos - screenCenter;

	if (glm::length(mouseDirection) == 0.f)
	{
		mouseDirection = { 1,0 };
	}
	else
	{
		mouseDirection = normalize(mouseDirection);
	}

	float spaceShipAngle = atan2(mouseDirection.y, -mouseDirection.x);

#pragma endregion


#pragma region handle bullets

	constexpr float playerShipSize = 50.f;
	constexpr float enemyShipSize = 50.f;

	if (platform::isLMousePressed())
	{
		Bullet b;

		b.position = data.playerPos;
		b.fireDirection = mouseDirection;
		b.speed = data.playerBulletSpeed;
		b.damage = data.playerDamage;

		PlaySound(shootSound);

		data.bullets.push_back(b);
	}

	for (int i = 0; i < data.bullets.size(); i++)
	{
		if (glm::distance(data.bullets[i].position, data.playerPos) > 5'000)
		{
			data.bullets.erase(data.bullets.begin() + i);
			i--;
			continue;
		}

		if (!data.bullets[i].isEnemy)
		{
			bool breakBothLoops = false;
			for (int e = 0; e < data.enemies.size(); e++)
			{
				if (intersectBullet(data.bullets[i].position, data.enemies[e].position, enemyShipSize))
				{
					//destoy bullet
					data.bullets.erase(data.bullets.begin() + i);
					data.enemies[e].health -= data.bullets[i].damage;
					if (data.enemies[e].health <= 0)
					{
						//handle item before enemy erase
						Item hp;
						hp.type = getRandomItemType();
						hp.position = data.enemies[e].position;
						data.items.push_back(hp);

						//kill enemy
						data.enemies.erase(data.enemies.begin() + e);
					}
					i--;
					breakBothLoops = true;
					continue;
				}
			}
			if (breakBothLoops)
				continue;
		}
		else
		{
			if (intersectBullet(data.bullets[i].position, data.playerPos, playerShipSize))
			{
				if (data.playerArmour > 0.f)
				{
					if (data.playerArmour >= data.bullets[i].damage)
					{
						data.playerArmour -= data.bullets[i].damage;
					}
					else
					{
						data.playerArmour = 0.f;
						data.playerHealth -= glm::abs(data.bullets[i].damage - data.playerArmour);
					}
				}
				else
				{
					data.playerHealth -= data.bullets[i].damage;
				}
				data.bullets.erase(data.bullets.begin() + i);
				i--;
				continue;
			}
		}

		data.bullets[i].update(deltaTime);
	}

	if (data.playerHealth <= 0)
	{
		//kill player
		restartGame();
	}
	else
	{
		//data.playerHealth += deltaTime * data.playerHealthRegen;
		data.playerHealth = glm::clamp(data.playerHealth, 0.f, data.playerMaxHealth);
	}

#pragma endregion


#pragma region handle enemies

	//if (data.enemies.size() < 15)
	//{
	//	data.spawnEnemyTimerSeconds -= deltaTime;

	//	if (data.spawnEnemyTimerSeconds < 0)
	//	{
	//		data.spawnEnemyTimerSeconds = rand() % 5 + 1;
	//		spawnEnemy();

	//		if (rand() % 3 == 0)
	//		{
	//			spawnEnemy();
	//		}
	//	}
	//}

	for (int i = 0; i < data.enemies.size(); i++)
	{
		if (glm::distance(data.enemies[i].position, data.playerPos) > 4000.f)
		{
			//kill enemy
			data.enemies.erase(data.enemies.begin() + i);
			i--;
			continue;
		}

		//enemy should shoot or not
		if (data.enemies[i].update(deltaTime, data.playerPos))
		{
			Bullet b;
			b.position = data.enemies[i].position;

			//spread bullets randomly based on distance
			glm::vec2 dir = data.enemies[i].viewDirection;
			// distance to player
			float dist = glm::distance(data.enemies[i].position, data.playerPos);
			// normalize distance into 0 → 1 range (tweak maxDist)
			float maxDist = 600.0f;
			float t = glm::clamp(dist / maxDist, 0.0f, 1.0f);
			float spread = data.enemies[i].minSpread + (data.enemies[i].maxSpread - data.enemies[i].minSpread) * t;
			dir.x += ((rand() % 100) / 100.f - 0.5f) * spread;
			dir.y += ((rand() % 100) / 100.f - 0.5f) * spread;
			b.fireDirection = glm::normalize(dir);

			b.isEnemy = true;
			b.speed = data.enemies[i].bulletSpeed;
			b.damage = data.enemies[i].damage;

			if (!IsSoundPlaying(shootSound)) PlaySound(shootSound);

			data.bullets.push_back(b);
		}
	}

#pragma endregion


#pragma region handle items

	for (int i = 0; i < data.items.size(); i++)
	{
		if (glm::distance(data.items[i].position, data.playerPos) <= data.items[i].itemPickupRange)
		{
			//destroy item gameobject
			data.items.erase(data.items.begin() + i);

			//apply item
			switch (data.items[i].type)
			{
			case ItemTypes::none:
				break;

			case ItemTypes::health:
				data.playerHealth += 50.f;
				break;

			case ItemTypes::armour:
				data.playerArmour += 10.f;
				break;

			default:
				break;
			}

			data.playerHealth = glm::clamp(data.playerHealth, 0.f, data.playerMaxHealth);
		}
	}

#pragma endregion


#pragma region render enemies

	for (auto& e : data.enemies)
	{
		e.render(renderer, spaceShipTexture, spaceShipAtlas, enemyShipSize);
		e.renderHealthBar(renderer, healthTexture, healthBarTexture, data.playerPos);
	}

#pragma endregion


#pragma region render ship

	renderSpaceShip(
		renderer,
		data.playerPos,
		playerShipSize,
		spaceShipTexture,
		spaceShipAtlas.get(3, 0),
		mouseDirection
	);
#pragma endregion


#pragma region render bullets

	for (auto& b : data.bullets)
	{
		b.render(renderer, bulletTexture, bulletAtlas);
	}

#pragma endregion


#pragma region render item

	for (auto& i : data.items)
	{
		gl2d::Texture tex = itemTypeToTexture[i.type];
		i.render(renderer, tex);
	}

#pragma endregion


#pragma region ui

	renderer.pushCamera();
	{
		glui::Frame f({ 0,0,w,h });

		glui::Box healthBox = glui::Box().xLeftPerc(0.65).yTopPerc(0.1).xDimensionPercentage(0.3).yAspectRatio(1.f / 8.f);

		renderer.renderRectangle(healthBox, healthBarTexture);

		glm::vec4 newRect = healthBox();
		newRect.z *= data.playerHealth;

		glm::vec4 textureCoords = { 0,1,1,0 };
		textureCoords.z *= data.playerHealth;

		renderer.renderRectangle(newRect, healthTexture, Colors_White, {}, {}, textureCoords);
	}
	renderer.popCamera();

#pragma endregion


	renderer.flush();


	//ImGui::ShowDemoWindow();

	ImGui::Begin("debug");

	ImGui::Text("Armour: %d", (int)data.playerArmour);
	ImGui::Text("Health: %d", (int)data.playerHealth);
	ImGui::Text("Bullets count: %d", (int)data.bullets.size());
	ImGui::Text("Enemies count: %d", (int)data.enemies.size());
	ImGui::Text("Items count: %d", (int)data.items.size());

	if (ImGui::Button("Spawn Enemy"))
	{
		spawnEnemy();
	}

	if (ImGui::Button("Spawn Item"))
	{
		Item hp;
		hp.position = data.playerPos;
		data.items.push_back(hp);
	}

	if (ImGui::Button("Restart Game"))
	{
		restartGame();
	}

	ImGui::SliderFloat("Player Health", &data.playerHealth, 0, 1);

	ImGui::End();


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
