#include "LegacyStar.h"

LegacyStar* LegacyStar::create(int color){
	LegacyStar* ret = new LegacyStar();
	if(ret && ret->initWithFile(ret->getImage(color))){
		ret->color = color;
		ret->selected = false;
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

char* LegacyStar::getImage(int color){
	switch(color){
	case color::BLUE:
		return "blue.png";
	case color::GREEN:
		return "green.png";
	case color::YELLOW:
		return "orange.png";
	case color::RED:
		return "red.png";
	case color::PURPLE:
		return "purple.png";
	}
}

bool LegacyStar::isSelected(){
	return selected;
}

int LegacyStar::getColor(){
	return color;
}

void LegacyStar::setDesPosition(const cocos2d::Point& p){
	desPosition = p;
}

void LegacyStar::updatePosition(){
	if(desPosition.y != getPositionY()){
		setPositionY(getPositionY() - MOVE_SPEED);
		if(getPositionY() < desPosition.y){
			setPositionY(desPosition.y);
		}
	}
	if(desPosition.x != getPositionX()){
		setPositionX(getPositionX() - MOVE_SPEED);
		if(getPositionX() < desPosition.x){
			setPositionX(desPosition.x);
		}
	}
}