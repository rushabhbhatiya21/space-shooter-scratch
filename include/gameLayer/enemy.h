#pragma once
#include <gl2d/gl2d.h>

struct Enemy {
	float speed = 200.f;
	float turnSpeed = 5.f;
	float fireThreashold = 0.8f;
	float fireRange = 1000.f;
	float firedTime = 2.f;
	float fireTimeReset = 0.5f;
	float bulletSpeed = 500.f;

	float life = 1.f;

	glm::ivec2 type = {};
	glm::vec2 position = {};

	glm::vec2 viewDirection = { 1,0 };

	void render(gl2d::Renderer2D &renderer, gl2d::Texture &sprites, gl2d::TextureAtlasPadding &atlas, float shipSize);

	void renderHealthBar(gl2d::Renderer2D& renderer, gl2d::Texture& healthTexture, gl2d::Texture& healthBarTexture, glm::vec2 playerPos);

	bool update(float deltaTime, glm::vec2 playerPos);
};