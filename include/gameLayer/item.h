#include <gl2d/gl2d.h>

struct Item
{
	glm::vec2 position = { 0,0 };

	void render(gl2d::Renderer2D& renderer, gl2d::Texture texture);
};