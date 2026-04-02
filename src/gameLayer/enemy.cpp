#pragma once
#include <enemy.h>
#include <tileRenderer.h>

void Enemy::render(gl2d::Renderer2D& renderer, gl2d::Texture& sprites, gl2d::TextureAtlasPadding& atlas, float shipSize)
{
	renderSpaceShip(renderer, position, shipSize, sprites, atlas.get(type.x, type.y), viewDirection);
}

void Enemy::renderHealthBar(gl2d::Renderer2D& renderer, gl2d::Texture& healthTexture, gl2d::Texture& healthBarTexture, glm::vec2 playerPos)
{
	if (glm::distance(playerPos, position) > 900.0f)
		return;

	float offsetY = 40.0f; // depends on your sprite size
	glm::vec2 hpPos = position - glm::vec2(0, offsetY);

	glm::vec2 size = { 30.0f, 5.0f }; // width, height

	glm::vec4 rect = {
		hpPos.x - size.x / 2.0f, // center align
		hpPos.y,
		size.x,
		size.y
	};

	renderer.renderRectangle(rect, healthBarTexture);

	glm::vec4 hpRect = rect;
	hpRect.z *= health / 1.f; // scale width

	glm::vec4 texCoords = { 0,1,1,0 };
	texCoords.z *= health / 1.f;

	renderer.renderRectangle(hpRect, healthTexture, Colors_White, {}, {}, texCoords);
}

bool Enemy::update(float deltaTime, glm::vec2 playerPos)
{
	glm::vec2 directionToPlayer = playerPos - position;
	float distance = glm::length(directionToPlayer);

	if (glm::length(directionToPlayer) == 0)
		directionToPlayer = { 1,0 };
	else
		directionToPlayer = glm::normalize(directionToPlayer);

	glm::vec2 newDirection = {};

	bool inRange = (distance <= fireRange);
	//bool facingPlayer = (glm::length(directionToPlayer + viewDirection) >= fireRange);
	bool facingPlayer = (glm::dot(directionToPlayer, viewDirection) >= fireThreashold);
	bool shoot = inRange && facingPlayer;

	if (shoot)
	{
		if (firedTime <= 0.f)
		{		
			//we can shoot
			firedTime = fireTimeReset;
		}
		else
		{
			shoot = 0;
		}

	}

	firedTime -= deltaTime;
	if (firedTime < 0) { firedTime = 0.f; }

	if (glm::length(directionToPlayer + viewDirection) <= 0.2)
	{
		if (rand() % 2)
		{
			newDirection = glm::vec2(directionToPlayer.y, -directionToPlayer.x);
		}
		else
		{
			newDirection = glm::vec2(-directionToPlayer.y, directionToPlayer.x);
		}
	}
	else
	{
		newDirection = deltaTime * turnSpeed * directionToPlayer + viewDirection;
	}

	float length = glm::length(newDirection);
	//viewDirection = glm::normalize(newDirection);
	length = glm::clamp(length, 0.1f, 3.f);

	viewDirection = glm::normalize(newDirection);

	position += viewDirection * deltaTime * speed * length;

	return shoot;
}