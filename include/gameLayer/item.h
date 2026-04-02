#include <gl2d/gl2d.h>

struct Item
{
	float itemPickupRange = 40.f;

	glm::vec2 position = { 0,0 };

	void render(gl2d::Renderer2D& renderer, gl2d::Texture texture);
};