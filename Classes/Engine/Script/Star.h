#ifndef __STAR__
#define __STAR__

#include "../Actor/BaseScriptComponent.h"
#include "cocos2d.h"

#include <memory>

class Star final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	Star(const Star&) = delete;
	Star(Star&&) = delete;
	Star& operator=(const Star&) = delete;
	Star& operator=(Star&&) = delete;
	
public:
	~Star();

	void randomize(int row_num, int col_num, float pos_x, float pos_y);
	void moveTo(float pos_x, float pos_y);
	bool canGroupWith(Star *star) const;

	cocos2d::Color4F getColor4F() const;

	float getPositionX() const;
	float getPositionY() const;

	void setVisible(bool visible);

	bool isSelected() const;
	void setSelected(bool selected);

	int getRowNum() const;
	void setRowNum(int row_num);
	int getColNum() const;
	void setColNum(int col_num);

	const static int WIDTH = 48;
	const static int HEIGHT = 48;

private:
	Star(Actor *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STAR__
