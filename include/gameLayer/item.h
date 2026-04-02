#include <gl2d/gl2d.h>

enum class ItemTypes
{
	none,
	health,
	armour,
	COUNT
};

ItemTypes getRandomItemType();

struct Item
{
	float itemPickupRange = 40.f;

	ItemTypes type = ItemTypes::none;

	glm::vec2 position = { 0,0 };

	void render(gl2d::Renderer2D& renderer, gl2d::Texture texture);
};