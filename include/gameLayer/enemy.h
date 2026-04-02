#pragma once
#include <gl2d/gl2d.h>

struct Enemy {
	float speed = 400.f;
	float turnSpeed = 5.f;
	float fireRange = 10.f;
	float firedTime = 1.f;
	float fireTimeReset = 0.1f;
	float bulletSpeed = 2000.f;

	float life = 1.f;

	glm::ivec2 type = {};
	glm::vec2 position = {};

	glm::vec2 viewDirection = { 1,0 };

	void render(gl2d::Renderer2D &renderer, gl2d::Texture &sprites, gl2d::TextureAtlasPadding &atlas, float shipSize);

	bool update(float deltaTime, glm::vec2 playerPos);
};