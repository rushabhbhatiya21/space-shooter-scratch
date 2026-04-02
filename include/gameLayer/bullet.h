#pragma once
#include <gl2d/gl2d.h>

struct Bullet
{
	bool isEnemy = 0;
	float bulletSize = 12.f;          // ↓ MUCH better (30 is huge)
	float speed = 900.f;              // ↓ slightly (less chaotic)
	float damage = 0.f;

	glm::vec2 position = {};
	glm::vec2 fireDirection = {};

	void render(gl2d::Renderer2D& renderer, gl2d::Texture bulletsTexture, gl2d::TextureAtlasPadding bulletsAtlas);

	void update(float deltaTime);
};