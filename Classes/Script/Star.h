#ifndef __STAR__
#define __STAR__

#include "Script.h"

#include <memory>

class Star final :public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	Star(const Star&) = delete;
	Star(Star&&) = delete;
	Star& operator=(const Star&) = delete;
	Star& operator=(Star&&) = delete;
	
public:
	~Star();

	void reset(int row_num, int col_num);

	void moveTo(int row_num, int col_num);

	bool canGroupWith(Star *star);

	int getColorNum() const;
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
	Star(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STAR__
