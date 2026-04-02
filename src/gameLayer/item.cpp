#include <item.h>
#include <gl2d/gl2d.h>

void Item::render(gl2d::Renderer2D& renderer, gl2d::Texture texture)
{
	renderer.renderRectangle(
		{ position - glm::vec2(50, 50), 100, 100},
		texture,
		Colors_White,
		{},
		0.f,
		{0,1,1,0}
	);
}