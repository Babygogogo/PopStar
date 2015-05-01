#include "Star.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../GameObject/SequentialInvoker.h"

#include <random>

struct Star::impl{
	impl(GameObject *game_object);
	~impl(){};

	std::string getImageFileName(int random_int);
	void setRandomPosition(int row_num, int col_num);

	float calculateMoveTime(int to_row_num, int to_col_num);
	cocos2d::Vec2 calculatePosition(int row_num, int col_num);

	GameObject *m_game_object{ nullptr };
	cocos2d::Sprite *m_sprite{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };

	int m_color_num{ 0 };
	bool m_is_selected{ false };
	int m_row_num{ 0 }, m_col_num{ 0 };

	const static float MOVE_SPEED;
	const static int ROWS_TOTAL = 10;

	static std::random_device rd;
	static std::default_random_engine random_engine;
	static std::uniform_int_distribution<> random_color_num;
	static std::uniform_int_distribution<> random_pos_offset;
};

const float Star::impl::MOVE_SPEED = 250.0f;

std::random_device Star::impl::rd;
std::default_random_engine Star::impl::random_engine(rd());
std::uniform_int_distribution<> Star::impl::random_color_num(0, 4);
std::uniform_int_distribution<> Star::impl::random_pos_offset(-100, 100);

Star::impl::impl(GameObject *game_object) :m_game_object(game_object)
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

void Star::impl::setRandomPosition(int row_num, int col_num)
{
	m_row_num = row_num;
	m_col_num = col_num;

	auto position = calculatePosition(m_row_num, m_col_num);
	position.x += random_pos_offset(random_engine);
	position.y += random_pos_offset(random_engine);
	m_sprite->setPosition(position);
}

cocos2d::Vec2 Star::impl::calculatePosition(int row_num, int col_num)
{
	return cocos2d::Point(col_num * Star::WIDTH + Star::WIDTH / 2, (ROWS_TOTAL - row_num) * Star::HEIGHT - Star::HEIGHT / 2);
}

float Star::impl::calculateMoveTime(int to_row_num, int to_col_num)
{
	auto current_pos = calculatePosition(m_row_num, m_col_num);
	auto to_pos = calculatePosition(to_row_num, to_col_num);
	auto delta_x = current_pos.x - to_pos.x;
	auto delta_y = current_pos.y - to_pos.y;

	return sqrt(delta_x * delta_x + delta_y * delta_y) / MOVE_SPEED;
}

Star::Star(GameObject *game_object) :Script("Star", game_object), pimpl(new impl(game_object))
{
	pimpl->m_sprite = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>();
	(pimpl->m_invoker = game_object->addComponent<SequentialInvoker>())->setInvokeContinuously(true);
}

Star::~Star()
{

}

void Star::reset(int row_num, int col_num)
{
	pimpl->m_color_num = pimpl->random_color_num(pimpl->random_engine);
	pimpl->m_sprite->setTexture(pimpl->getImageFileName(pimpl->m_color_num));

	pimpl->m_sprite->setVisible(true);
	pimpl->m_is_selected = false;

	pimpl->setRandomPosition(row_num, col_num);
	pimpl->m_invoker->addFiniteTimeAction(cocos2d::MoveTo::create(0.7f, pimpl->calculatePosition(row_num, col_num)));
}

void Star::moveTo(int row_num, int col_num)
{
	if (row_num == pimpl->m_row_num && col_num == pimpl->m_col_num)
		return;

	pimpl->m_invoker->addFiniteTimeAction(cocos2d::MoveTo::create(pimpl->calculateMoveTime(row_num, col_num), pimpl->calculatePosition(row_num, col_num)));
	pimpl->m_row_num = row_num;
	pimpl->m_col_num = col_num;
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

bool Star::canGroupWith(Star *star)
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
