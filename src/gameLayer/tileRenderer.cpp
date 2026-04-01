#pragma once
#include <tileRenderer.h>

void TileRenderer::render(gl2d::Renderer2D &renderer) {

	auto viewRect = renderer.getViewRect();

	glm::vec2 paralaxDisance = { viewRect.x,viewRect.y };
	paralaxDisance *= -paralaxStrength;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			int posX = i + int((viewRect.x - paralaxDisance.x) / backgrounndSize);
			int posY = j + int((viewRect.y - paralaxDisance.y) / backgrounndSize);

			renderer.renderRectangle(
				glm::vec4{ posX, posY, 1,1 } * backgrounndSize +
				glm::vec4(paralaxDisance, 0, 0),
				texture
			);
		}
	}

	renderer.renderRectangle({ 0,0,10000,10000 }, texture);
}

void renderSpaceShip(
	gl2d::Renderer2D& renderer,
	glm::vec2 position,
	float size,
	gl2d::Texture texture,
	glm::vec4 uvs,
	glm::vec2 viewDirection
)
{
	float angle = atan2(viewDirection.y, -viewDirection.x);

	renderer.renderRectangle(
		{ position - glm::vec2(size / 2, size / 2), size, size },
		texture,
		Colors_White,
		{},
		glm::degrees(angle) + 90.f,
		uvs
	);
}