#include <item.h>
#include <gl2d/gl2d.h>

ItemTypes getRandomItemType() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dist(1, static_cast<int>(ItemTypes::COUNT) - 1);

	return static_cast<ItemTypes>(dist(gen));
}


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