#include "Star.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../GameObject/SequentialInvoker.h"

#include <random>

struct Star::impl
{
	impl(GameObject *game_object);
	~impl();

	std::string getImageFileName(int random_int);
	void setRandomPosition(float pos_x, float pos_y);

	float calculateMoveTime(float to_pos_x, float to_pos_y, float speed);
	void moveTo(float pos_x, float pos_y, float speed = Star::impl::MOVE_SPEED);

	cocos2d::Sprite *m_sprite{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };

	int m_color_num{ 0 };
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

Star::impl::impl(GameObject *game_object)
{
	m_sprite = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>();
	
	m_invoker = game_object->addComponent<SequentialInvoker>();
	m_invoker->setInvokeContinuously(true);
}

Star::impl::~impl()
{

}

std::string Star::impl::getImageFileName(int color_num)
{
	switch (color_num % 5){
	case 0:
		return "blue.png";
	case 1:
		return "green.png";
	case 2:
		return "orange.png";
	case 3:
		return "red.png";
	case 4:
		return "purple.png";
	default:
		return{};
	}
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

Star::Star(GameObject *game_object) :Script("Star", game_object), pimpl(new impl(game_object))
{

}

Star::~Star()
{

}

void Star::randomize(int row_num, int col_num, float pos_x, float pos_y)
{
	pimpl->m_color_num = pimpl->random_color_num(pimpl->random_engine);
	pimpl->m_sprite->setTexture(pimpl->getImageFileName(pimpl->m_color_num));
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
	return this != star && pimpl->m_color_num == star->pimpl->m_color_num;
}

int Star::getColorNum() const
{
	return pimpl->m_color_num;
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
