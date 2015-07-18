#include "Star.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Actor/SequentialInvoker.h"
#include "cocos2d.h"
#include <random>

struct Star::impl
{
	impl(Actor *game_object);
	~impl();

	void setRandomColor();
	std::string getImageFileName();
	void setRandomPosition(float pos_x, float pos_y);

	float calculateMoveTime(float to_pos_x, float to_pos_y, float speed);
	void moveTo(float pos_x, float pos_y, float speed = Star::impl::MOVE_SPEED);

	cocos2d::Sprite *m_sprite{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };

	enum class Color{ BLUE, GREEN, ORANGE, RED, PURPLE } m_color;
	cocos2d::Color4F m_color4f;
	std::string m_texture_name;
	bool m_is_selected{ false };
	int m_row_num{ 0 }, m_col_num{ 0 };
	float m_pos_x{ 0.0f }, m_pos_y{ 0.0f };

	const static float MOVE_SPEED;

	static std::random_device rd;
	static std::default_random_engine random_engine;
	static std::uniform_int_distribution<> random_color_num;
	static std::uniform_int_distribution<> random_pos_offset;
};

const float Star::impl::MOVE_SPEED = 250.0f;

std::random_device Star::impl::rd;
std::default_random_engine Star::impl::random_engine(rd());
std::uniform_int_distribution<> Star::impl::random_color_num(0, 4);
std::uniform_int_distribution<> Star::impl::random_pos_offset(-135, 135);

Star::impl::impl(Actor *game_object)
{
	m_sprite = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>();
	
	m_invoker = game_object->addComponent<SequentialInvoker>();
	m_invoker->setInvokeContinuously(true);
}

Star::impl::~impl()
{

}

void Star::impl::setRandomColor()
{
	switch (random_color_num(random_engine)){
	case 0:	m_color = Color::BLUE;		m_texture_name = "blue.png";	m_color4f = cocos2d::Color4F(84.0f / 255.0f, 203.0f / 255.0f, 254.0f / 255.0f, 1);	break;
	case 1:	m_color = Color::GREEN;		m_texture_name = "green.png";	m_color4f = cocos2d::Color4F(132.0f / 255.0f, 226.0f / 255.0f, 111.0f / 255.0f, 1);	break;
	case 2:	m_color = Color::ORANGE;	m_texture_name = "orange.png";	m_color4f = cocos2d::Color4F(253.0f / 255.0f, 234.0f / 255.0f, 84.0f / 255.0f, 1);	break;
	case 3:	m_color = Color::PURPLE;	m_texture_name = "purple.png";	m_color4f = cocos2d::Color4F(189.0f / 255.0f, 78.0f / 255.0f, 253.0f / 255.0f, 1);	break;
	case 4:	m_color = Color::RED;		m_texture_name = "red.png";		m_color4f = cocos2d::Color4F(253.0f / 255.0f, 80.0f / 255.0f, 126.0f / 255.0f, 1);	break;
	default:m_color = Color::BLUE;		m_texture_name = "blue.png";	m_color4f = cocos2d::Color4F(84.0f / 255.0f, 203.0f / 255.0f, 254.0f / 255.0f, 1);	break;
	};

	m_sprite->setTexture(m_texture_name);
}

void Star::impl::setRandomPosition(float pos_x, float pos_y)
{
	m_pos_x = pos_x + random_pos_offset(random_engine);
	m_pos_y = pos_y + random_pos_offset(random_engine);

	m_sprite->setPosition(m_pos_x, m_pos_y);
}

float Star::impl::calculateMoveTime(float to_pos_x, float to_pos_y, float speed)
{
	auto delta_x = m_pos_x - to_pos_x;
	auto delta_y = m_pos_y - to_pos_y;

	return sqrt(delta_x * delta_x + delta_y * delta_y) / speed;
}

void Star::impl::moveTo(float pos_x, float pos_y, float speed /*= Star::impl::MOVE_SPEED*/)
{
	if (pos_x == m_pos_x && pos_y == m_pos_y)
		return;

	m_invoker->addMoveTo(calculateMoveTime(pos_x, pos_y, speed), pos_x, pos_y);
	m_pos_x = pos_x;
	m_pos_y = pos_y;
}

Star::Star(Actor *game_object) :BaseScriptComponent("Star", game_object), pimpl(new impl(game_object))
{

}

Star::~Star()
{

}

void Star::randomize(int row_num, int col_num, float pos_x, float pos_y)
{
	pimpl->setRandomColor();
	pimpl->m_sprite->setVisible(true);

	pimpl->m_is_selected = false;
	pimpl->m_row_num = row_num;
	pimpl->m_col_num = col_num;

	pimpl->setRandomPosition(pos_x, pos_y);
	pimpl->moveTo(pos_x, pos_y, Star::impl::MOVE_SPEED * 0.6f);
}

void Star::moveTo(float pos_x, float pos_y)
{
	pimpl->moveTo(pos_x, pos_y);
}

bool Star::isSelected() const
{
	return pimpl->m_is_selected;
}

void Star::setSelected(bool selected)
{
	pimpl->m_is_selected = selected;
}

int Star::getRowNum() const
{
	return pimpl->m_row_num;
}

void Star::setRowNum(int row_num)
{
	pimpl->m_row_num = row_num;
}

int Star::getColNum() const
{
	return pimpl->m_col_num;
}

void Star::setColNum(int col_num)
{
	pimpl->m_col_num = col_num;
}

bool Star::canGroupWith(Star *star) const
{
	return this != star && pimpl->m_color == star->pimpl->m_color;
}

cocos2d::Color4F Star::getColor4F() const
{
	return pimpl->m_color4f;
}

float Star::getPositionX() const
{
	return pimpl->m_sprite->getPositionX();
}

float Star::getPositionY() const
{
	return pimpl->m_sprite->getPositionY();
}

void Star::setVisible(bool visible)
{
	pimpl->m_sprite->setVisible(visible);
}
