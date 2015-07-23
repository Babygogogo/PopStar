#ifndef __STAR__
#define __STAR__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d
{
	struct Color4F;
}

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
	Star(Actor *game_object);
	~Star();

	static const std::string Type;

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

	virtual const std::string & getType() const override;

	const static int WIDTH = 48;
	const static int HEIGHT = 48;

private:

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STAR__
