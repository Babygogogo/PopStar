#ifndef _STAR_H_
#define _STAR_H_

#include "cocos2d.h"

class LegacyStar : public cocos2d::Sprite{
public:
	enum color{
		GREEN,
		RED,
		YELLOW,
		PURPLE,
		BLUE
	};
	static LegacyStar* create(int color);
	bool isSelected();
	inline void setSelected(bool b){selected = b;}
	int getColor();
	void setDesPosition(const cocos2d::Point& p);
	inline cocos2d::Point getDesPosition(){ return desPosition; }
	void updatePosition();
	inline int getRowNum(){return index_i;}
	inline int getColNum(){return index_j;}
	inline void setMatrixIndex(int i,int j){index_i = i;index_j = j;}
private:
	char* getImage(int color);

public:
	const static int MOVE_SPEED = 4;
	static const int COLOR_MAX_NUM = 5;
	static const int STAR_WIDTH = 48;
	static const int STAR_HEIGHT = 48;
private:
	int color;
	cocos2d::Point desPosition;
	bool selected;
	int index_i;
	int index_j;
};
#endif