#pragma once
#include <gl2d/gl2d.h>

struct Enemy {
	float speed = 220.f;              // slight ↑ (keeps pressure)
	float turnSpeed = 7.f;            // ↑ reacts faster to player
	float fireThreashold = 0.9f;      // ↑ must aim properly (feels fair)
	float fireRange = 600.f;          // ↓ no sniping from far away

	float firedTime = 0.f;            // start ready to shoot
	float fireTimeReset = 1.2f;       // ↑ slower fire rate (fairness)

	float minSpread = 0.02f;  // very accurate up close
	float maxSpread = 0.15f;  // inaccurate at range

	float bulletSpeed = 700.f;        // ↑ harder to dodge but not insane
	float health = 80.f;                // ↓ less tanky (more satisfying kills)
	float damage = 10.f;

	glm::ivec2 type = {};
	glm::vec2 position = {};

	glm::vec2 viewDirection = { 1,0 };

	void render(gl2d::Renderer2D &renderer, gl2d::Texture &sprites, gl2d::TextureAtlasPadding &atlas, float shipSize);

	void renderHealthBar(gl2d::Renderer2D& renderer, gl2d::Texture& healthTexture, gl2d::Texture& healthBarTexture, glm::vec2 playerPos);

	bool update(float deltaTime, glm::vec2 playerPos);
};