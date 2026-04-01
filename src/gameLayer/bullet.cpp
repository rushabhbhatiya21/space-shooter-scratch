#pragma once
#include <bullet.h>

void Bullet::render(gl2d::Renderer2D& renderer, gl2d::Texture bulletsTexture, gl2d::TextureAtlasPadding bulletsAtlas)
{
	float bulletAngle = atan2(fireDirection.y, -fireDirection.x);
	float degrees = glm::degrees(bulletAngle) + 90.f;

	glm::vec4 textureCoords = bulletsAtlas.get(1, 1);

	if (isEnemy)
	{
		textureCoords = bulletsAtlas.get(0, 0);
	}

	renderer.renderRectangle(
		{ position + glm::vec2(0,0), bulletSize, bulletSize },
		bulletsTexture,
		Colors_White,
		{},
		degrees,
		textureCoords
	);
}

void Bullet::update(float deltaTime)
{
	position += fireDirection * deltaTime * 1500.f;
}